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

Server::Server() {}

Server::Server(const Server &other) { *this = other; }

Server::~Server() {
	std::vector<struct pollfd>::iterator it;
	for (it = _pollFd.begin(); it != _pollFd.end(); it++) {
		close(it->fd);
	}
}

Server &Server::operator=(const Server &other) {
	if (this == &other)
		return *this;
	_listenerSockets = other._listenerSockets;
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
	for (size_t i = 0; i < _listenerSockets.size(); i++) {
		if (!_tryListenSocket(_listenerSockets[i])) {
			return false;
		}
		std::cout << _listenerSockets[i].getIPAndPort() << std::endl;
	}
	while (true) {
		coloredLog("Waiting for a request...", "", GREEN);
		if (poll(_pollFd.data(), _pollFd.size(), CLIENT_TIMEOUT_MS) == -1) {
			_printError("server.listen poll() failed");
			return false;
		}
		_check_revents_sockets();
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
		serverSocket.closeFD();
		return false;
	}
	_listenerSockets.push_back(serverSocket);
	_pollFd.push_back(serverSocket.getPollFd(POLLIN));
	_virtualServers.push_back(virtualServer);
	return true;
}

bool Server::_existListenerSocket(const std::string &IPAddress, unsigned short port) {
	std::vector<Socket>::const_iterator it;
	for (it = _listenerSockets.begin(); it != _listenerSockets.end(); it++) {
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

void Server::_check_revents_sockets(void) {
	for (size_t i = 0; i < _pollFd.size(); i++) {
		Client* client = _getClientFromFD(_pollFd[i].fd);
		if (client != NULL && client->getMSSinceLastActivity() > PERSISTENCE_SLEEP_MS) {
			std::cout << "A client socket has been timeout" << std::endl;
			close(_pollFd[i].fd);
			_pollFd.erase(_pollFd.begin() + i);
			_clients.erase(_clients.begin() + (i - _listenerSockets.size()));
			i--;
			continue;
		}
		if ((_pollFd[i].revents & POLLIN) == 0)
			continue;
		if (i  < _listenerSockets.size()) {
			_accept_new_client(_pollFd[i]);
		} else if (_read_persistent_connection(i) == 0) {
			i--;
		}
	}
}

bool Server::_accept_new_client(struct pollfd listener) {
	struct sockaddr_in	clientAddress;
	socklen_t clientAddressLen = sizeof(clientAddress);
	struct sockaddr_in	serverAddress;
	socklen_t serverAddressLen = sizeof(serverAddressLen);

	int clientFD = accept(listener.fd,
						  (struct sockaddr*)&clientAddress, &clientAddressLen);
	if (clientFD == -1) {
		_printError("server.listen accept() failed");
		return false;
	}
	// TODO: secure

	getsockname(clientFD, (struct sockaddr*)&serverAddress, &serverAddressLen);
//	char serverIP[INET_ADDRSTRLEN];
//	std::cout << inet_ntop(AF_INET, &(serverAddress.sin_addr.s_addr), serverIP, INET_ADDRSTRLEN) << std::endl;
//	std::cout << "here" << serverIP << std::endl;
	std::cout << Server::ft_inet_ntoa(serverAddress.sin_addr.s_addr) << std::endl;

	_clients.push_back(Client(clientFD, clientAddress, serverAddress));
	_pollFd.push_back((struct pollfd) {.fd = clientFD, .events = POLLIN, .revents = 0});
	std::cout << "A new client is connected" << std::endl;
	return true;
}

ssize_t	Server::_read_persistent_connection(size_t client_index) {
	Client* client = _getClientFromFD(_pollFd[client_index].fd);

	coloredLog("Request received :", "", BLUE);
	char buffer[10];
	memset(buffer, 0, sizeof(buffer));
	std::string completeClientRequest;
	completeClientRequest = "";
	ssize_t bytesRead = (ssize_t)(sizeof(buffer));
//	while (bytesRead >= (ssize_t)(sizeof(buffer))) {
		bytesRead = read(_pollFd[client_index].fd, buffer, sizeof(buffer));
//		if (bytesRead == -1)
//			break;
		std::string tmp;
		tmp.append(buffer, bytesRead);
		client->appendRawRequest(tmp);
//	}
	if (bytesRead == 0)
	{
		std::cout << "A client socket has been close" << std::endl;
		close(_pollFd[client_index].fd);
		_pollFd.erase(_pollFd.begin() + client_index);
		return bytesRead;
	}
	if (client->checkRequestValidity() != NOT_COMPLETE){
//		HttpRequest httpRequest = HttpRequest(completeClientRequest);
		HttpRequest httpRequest = client->getRequest();
//		httpRequest.displayRequest();
		HttpResponse httpResponse;
		std::string response = httpResponse.getResponse((*this), httpRequest);
		write(_pollFd[client_index].fd, response.c_str(), response.size());
		coloredLog("Response sent", "[" + toString(client_index) + "] :", BLUE);
	}
	return bytesRead;
}

void Server::_printError(const std::string &error) {
	std::cerr << "webserver: " << error << " ("
			  << errno << ": " << strerror(errno) << ")" << std::endl;
}

Client*	Server::_getClientFromFD(int fd) {
	std::vector<Client>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++) {
		if (it->getFD() == fd)
			return &(*it);
	}
	return NULL;
}


std::string Server::ft_inet_ntoa(uint32_t s_addr) {
	std::stringstream	ip;

	s_addr = ntohl(s_addr);
	ip << ((s_addr & 0xff000000) >> 24)
	   << '.' << ((s_addr & 0xff0000) >> 16)
	   << '.' << ((s_addr & 0xff00) >> 8)
	   << '.' << (s_addr & 0xff);
	return (ip.str());
}