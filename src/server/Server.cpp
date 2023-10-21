#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cerrno>

#include "server/Server.hpp"
#include "server/Client.hpp"
#include "HttpMessages/AHttpMessage.hpp"
#include "HttpMessages/HttpRequest.hpp"
#include "HttpMessages/HttpResponse.hpp"
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
		if (it->getIPAddress() == UNSPECIFIED_ADDRESS && !_createVirtualServer(*it))
			return false;
	}
	for (it = virtualServers.begin(); it != virtualServers.end(); it++) {
		if (it->getIPAddress() != UNSPECIFIED_ADDRESS && !_createVirtualServer(*it))
			return false;
	}
	return true;
}

bool Server::listen() {
	coloredLog("Start listening...", "", GREEN);
	for (size_t i = 0; i < _listeners.size(); i++) {
		if (!_tryListenSocket(_listeners[i])) {
			return false;
		}
		std::cout << _listeners[i].getIPAndPort() << std::endl;
	}
	while (!exit) {
		coloredLog("Waiting for a request...", "", GREEN);
		if (poll(_pollFd.data(), _pollFd.size(), CLIENT_TIMEOUT_MS) == -1 && !exit) {
			_printError("server.listen poll() failed");
			return false;
		}
		_handleSockets();
	}
	return true;
}

VirtualServer *Server::getVirtualServer(const std::string &serverName) {
	for (size_t i = 0; i < _virtualServers.size(); i++) {
		if (_virtualServers[i].getServerName()[0] == serverName) //todo : ne check que le premier serverName..
			return &_virtualServers[i];
	}
	return NULL;
}

void Server::displayVirtualServers() {
	coloredLog( "Webserv virtual servers(hosts): ", "", BLUE);
	for (size_t i = 0; i < _virtualServers.size(); i++) {
		coloredLog("\thost [" + toString(i) + "]: ", _virtualServers[i].getServerName()[0], PURPLE);
	}
}

bool Server::_createVirtualServer(const VirtualServer &virtualServer) {
	if (_existListenerSocket(virtualServer.getIPAddress(), virtualServer.getPort())) {
		_virtualServers.push_back(virtualServer);
		return true;
	}
	Socket	serverSocket = Socket();
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

bool Server::_existListenerSocket(const std::string &IPAddress, unsigned short port) {
	std::vector<Socket>::const_iterator it;
	for (it = _listeners.begin(); it != _listeners.end(); it++) {
		if ((it->getIPAddress() == UNSPECIFIED_ADDRESS && it->getPort() == port)
			|| (it->getIPAddress() == IPAddress && it->getPort() == port))
			return true;
	}
	return false;
}

bool Server::_tryBindSocket(Socket &socket, const std::string& IPAddress,
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
		_printError("socket.listening() to " + socket.getIPAndPort() + " failed");
		ftSleep(PERSISTENCE_SLEEP_MS);
	}
	return false;
}

void Server::_handleSockets() {
	for (size_t i = 0; i < _pollFd.size();) {
		bool isDisconnect = false;
		if (i >= _listeners.size()) {
			isDisconnect = !_handleClient(_pollFd[i], i - _listeners.size());
		}
		else if ((_pollFd[i].revents & POLLIN) == 1 && i < _listeners.size()) {
			_acceptNewClient(_pollFd[i]);
		}
		if (isDisconnect)
			_pollFd.erase(_pollFd.begin() + i);
		else
			i++;
	}
}

bool Server::_acceptNewClient(struct pollfd listener) {
	struct sockaddr_in	serverAddress;
	struct sockaddr_in	clientAddress;
	socklen_t			serverAddressLength = sizeof(serverAddress);
	socklen_t			clientAddressLength = sizeof(clientAddressLength);

	int clientSocket = accept(listener.fd, (struct sockaddr*) &clientAddress,
							  &clientAddressLength);
	if (clientSocket == -1) {
		_printError("server.listen accept() failed");
		return false;
	}
	if (getsockname(clientSocket, (struct sockaddr*) &serverAddress,
					&serverAddressLength) == -1) {
		_printError("server.listen getsockname() failed");
		close(clientSocket);
		return false;
	}
	Client client(clientSocket, clientAddress, serverAddress);
	_clients.push_back(client);
	_pollFd.push_back((struct pollfd) {.fd = clientSocket, .events = POLLIN, .revents = 0});
	std::cout << "A new client is connected" << std::endl;
	return true;
}

bool	Server::_handleClient(struct pollfd& pollSocket, size_t clientIndex) {
	std::vector<Client>::iterator	clientIt = _clients.begin() + clientIndex;
	Client& client = *clientIt;
	if (client.getMSSinceLastActivity() >= CLIENT_TIMEOUT_MS) {
		client.disconnect();
		_clients.erase(clientIt);
		return false;
	}
	if ((pollSocket.revents & POLLIN) == 0)
		return true;
	if (!_readClientRequest(client)) {
		client.disconnect();
		_clients.erase(clientIt);
		return false;
	}
	enum REQUEST_VALIDITY validity = client.checkRequestValidity();
	if (validity == INVALID || validity == VALID_and_COMPLETE)
		_sendClientRequest(client);
	return true;
}

bool Server::_readClientRequest(Client &client) {
	char buffer[1024];
	std::string	strRequest;

	ssize_t bytesRead = recv(client.getFD(), buffer, sizeof(buffer), 0);
	if (bytesRead == -1)
		return true;
	if (bytesRead == 0)
		return false;
	strRequest.append(buffer, bytesRead);
	client.appendRawRequest(strRequest);
	return true;
}

void Server::_sendClientRequest(Client &client) {
	HttpRequest httpRequest = client.getRequest();
//		httpRequest.displayRequest();
	HttpResponse httpResponse;
	std::string response = httpResponse.getResponse((*this), httpRequest);
	send(client.getFD(), response.c_str(), response.size(), 0);
//	coloredLog("Response sent", "[" + toString(client_index) + "] :", BLUE);
}

void Server::_printError(const std::string &error) {
	std::cerr << "webserver: " << error << " ("
			  << errno << ": " << strerror(errno) << ")" << std::endl;
}
