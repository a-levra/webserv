#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <algorithm>

#include "server/Server.hpp"
#include "server/Client.hpp"
#include "HttpMessages/AHttpMessage.hpp"
#include "HttpMessages/HttpRequest.hpp"
#include "HttpMessages/HttpResponse.hpp"
#include "logger/logging.hpp"
#include "utils/utils.hpp"

bool Server::exit = false;

Server::Server() {}

Server::Server(const Server &other) { *this = other; }

Server::~Server() {
	std::vector<Socket>::iterator socketIt;
	for (socketIt = _listeners.begin(); socketIt != _listeners.end();
		 socketIt++)
		socketIt->disconnect();
	std::vector<Client>::iterator clientIt;
	for (clientIt = _clients.begin(); clientIt != _clients.end(); clientIt++)
		clientIt->disconnect();
}

Server &Server::operator=(const Server &other) {
	if (this == &other)
		return *this;
	_listeners = other._listeners;
	_pollFd = other._pollFd;
	_virtualServers = other._virtualServers;
	return *this;
}

bool Server::addVirtualServers(const std::vector<VirtualServer> &virtualServers) {
	std::vector<VirtualServer>::const_iterator it;
	for (it = virtualServers.begin(); it != virtualServers.end(); it++) {
		if (it->getIPAddress() == UNSPECIFIED_ADDRESS
			&& !_createVirtualServer(*it))
			return false;
	}
	for (it = virtualServers.begin(); it != virtualServers.end(); it++) {
		if (it->getIPAddress() != UNSPECIFIED_ADDRESS
			&& !_createVirtualServer(*it))
			return false;
	}
	return true;
}

bool Server::listen() {
	logging::info("start listening");
	for (size_t i = 0; i < _listeners.size(); i++) {
		if (!_tryListenSocket(_listeners[i])) {
			return false;
		}
		logging::info(_listeners[i].getIPAndPort());
	}
	while (!exit) {
		if (poll(_pollFd.data(), _pollFd.size(), CLIENT_TIMEOUT_MS) == -1) {
			if (exit) {
				return true;
			}
			_printError("server.listen poll() failed");
			return false;
		}
		_handleSockets();
	}
	return true;
}

VirtualServer *Server::getVirtualServer(const std::string &IPAddress,
										const unsigned short port,
										const std::string &serverName) {
	std::vector<VirtualServer>::iterator vs;
	logging::debug(B_PURPLE "Host requested: " THIN + serverName + COLOR_RESET);
	logging::debug(
		B_PURPLE "Client entry point: " THIN + IPAddress + ":" + toString(port)
			+ THIN);
	for (vs = _virtualServers.begin(); vs != _virtualServers.end(); vs++) {
		bool DoesNotMatchIpAdress = (vs->getIPAddress() != IPAddress
			&& vs->getIPAddress() != "0.0.0.0");
		bool DoesNotMatchPort = (vs->getPort() != port);
		if (DoesNotMatchIpAdress || DoesNotMatchPort)
			continue;
		std::vector<std::string> serverNames = vs->getServerName();
		std::vector<std::string>::iterator serverNameIt =
			std::find(serverNames.begin(), serverNames.end(), serverName);
		if (serverNameIt != serverNames.end()) {
			logging::debug(B_GREEN "Host found: " THIN + *serverNameIt + " "
							   + vs->getIPAddress() + ":"
							   + toString(vs->getPort()) + "" COLOR_RESET);
			return &(*vs);
		}
	}
	for (vs = _virtualServers.begin(); vs != _virtualServers.end(); vs++) {
		if ((vs->getIPAddress() == IPAddress || vs->getIPAddress() == "0.0.0.0")
			&& vs->getPort() == port)
			return &(*vs);
	}
	logging::debug(B_RED "No hosts found" COLOR_RESET);
	return NULL;
}

void Server::displayVirtualServers() {
	logging::debug("Webserv virtual servers(hosts): ");
	for (size_t i = 0; i < _virtualServers.size(); i++) {
		logging::debug("\thost [" + toString(i) + "]: "
						   + _virtualServers[i].getServerName()[0]);
	}
}

bool Server::_createVirtualServer(const VirtualServer &virtualServer) {
	if (_existListenerSocket(virtualServer.getIPAddress(),
							 virtualServer.getPort())) {
		_virtualServers.push_back(virtualServer);
		return true;
	}
	Socket serverSocket = Socket();
	if (!serverSocket.initialize()) {
		_printError("socket.initialize() failed");
		return false;
	}
	if (!_tryBindSocket(serverSocket, virtualServer.getIPAddress(),
						virtualServer.getPort())) {
		serverSocket.disconnect();
		return false;
	}
	_listeners.push_back(serverSocket);
	_pollFd.push_back(serverSocket.getPollFd(POLLIN));
	_virtualServers.push_back(virtualServer);
	return true;
}

bool Server::_existListenerSocket(const std::string &IPAddress,
								  unsigned short port) {
	std::vector<Socket>::const_iterator it;
	for (it = _listeners.begin(); it != _listeners.end(); it++) {
		if ((it->getIPAddress() == UNSPECIFIED_ADDRESS && it->getPort() == port)
			|| (it->getIPAddress() == IPAddress && it->getPort() == port))
			return true;
	}
	return false;
}

bool Server::_tryBindSocket(Socket &socket, const std::string &IPAddress,
							unsigned short port) {
	for (int i = 0; i < PERSISTENCE_TRIALS; i++) {
		if (socket.binding(IPAddress, port))
			return true;
		_printError("socket.binding() to " + IPAddress + ":"
						+ toString(port) + " failed");
		ftSleep(PERSISTENCE_SLEEP_MS);
	}
	return false;
}

bool Server::_tryListenSocket(Socket &socket) {
	for (int i = 0; i < PERSISTENCE_TRIALS; i++) {
		if (socket.listening())
			return true;
		_printError(
			"socket.listening() to " + socket.getIPAndPort() + " failed");
		ftSleep(PERSISTENCE_SLEEP_MS);
	}
	return false;
}

void Server::_handleSockets() {
	for (size_t i = 0; i < _pollFd.size();) {
		bool isDisconnect = false;
		if (i >= _listeners.size())
			isDisconnect = !_handleClient(_pollFd[i], i - _listeners.size());
		else if ((_pollFd[i].revents & POLLIN) == 1 && i < _listeners.size())
			_acceptNewClient(_pollFd[i]);
		if (isDisconnect) {
			_pollFd.erase(_pollFd.begin() + i);
			logging::debug(B_BLUE "a client has been disconnected" THIN);
			logging::debug("------------------------------");
		} else
			i++;
	}
}

bool Server::_acceptNewClient(struct pollfd listener) {
	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;
	socklen_t serverAddressLength = sizeof(serverAddress);
	socklen_t clientAddressLength = sizeof(clientAddressLength);

	int clientSocket = accept(listener.fd, (struct sockaddr *) &clientAddress,
							  &clientAddressLength);
	if (clientSocket == -1) {
		_printError("server.listen accept() failed");
		return false;
	}
	if (getsockname(clientSocket, (struct sockaddr *) &serverAddress,
					&serverAddressLength) == -1) {
		_printError("server.listen getsockname() failed");
		close(clientSocket);
		return false;
	}
	Client client(clientSocket, clientAddress, serverAddress);
	_clients.push_back(client);
	_pollFd.push_back((struct pollfd) {.fd = clientSocket, .events = POLLIN, .revents = 0});
	logging::debug("a new client is connected");
	return true;
}

bool Server::_handleClient(struct pollfd &pollSocket, size_t clientIndex) {
	std::vector<Client>::iterator clientIt = _clients.begin() + clientIndex;
	Client &client = *clientIt;
	if (client.getMSSinceLastActivity() >= CLIENT_TIMEOUT_MS) {
		_disconnectClient(clientIt);
		return false;
	}
	if ((pollSocket.revents & POLLIN) == 0)
		return true;
	if (!_readClientRequest(client)) {
		_disconnectClient(clientIt);
		return false;
	}
	client.checkRequestValidity();
	if (client.getRequest().canSendResponse() && !_sendClientRequest(client)) {
		_disconnectClient(clientIt);
		return false;
	}
	return true;
}

bool Server::_readClientRequest(Client &client) {
	char buffer[READ_BUFFER_SIZE];
	std::string strRequest;

	ssize_t bytesRead = recv(client.getFD(), buffer, sizeof(buffer), 0);
	if (bytesRead == -1)
		return true;
	if (bytesRead == 0)
		return false;
	strRequest.append(buffer, bytesRead);
	client.appendRawRequest(strRequest);
	client.updateLastActivity();
	return true;
}

bool Server::_sendClientRequest(Client &client) {
	HttpRequest httpRequest = client.getRequest();
	HttpResponse httpResponse(httpRequest);
	std::string response = httpResponse.getResponse((*this), client);
	if (!response.empty()) {
		send(client.getFD(), response.c_str(), response.size(), MSG_NOSIGNAL);
		logging::info(client.getEntryIPAddress() + ":" +
			toString(client.getEntryPort()) + " " + httpRequest.getMethod()
						  + " " +
			httpRequest.getRequestUri());
		client.setRawRequest("");
	}
	if (httpResponse.getStatusCode() == PAYLOAD_TOO_LARGE) {
		return false;
	}
	return true;
}

void Server::_disconnectClient(std::vector<Client>::iterator clientIt) {
	if (clientIt == _clients.end())
		return;
	clientIt->disconnect();
	_clients.erase(clientIt);
}

void Server::_printError(const std::string &error) {
	logging::error(error + " (" + toString(errno) + ": "
					   + strerror(errno) + ")");
}
