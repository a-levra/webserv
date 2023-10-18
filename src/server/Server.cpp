#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cerrno>

#include "server/Server.hpp"
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
	return (*this);
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
		if (poll(_pollFd.data(), _pollFd.size(), -1) == -1) {
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
	std::string tempIPAddress;

	if (IPAddress == LOCALHOST)
		tempIPAddress = LOOPBACK_IP;
	else
		tempIPAddress = IPAddress;
	std::vector<Socket>::const_iterator it;
	for (it = _listenerSockets.begin(); it != _listenerSockets.end(); it++) {
		if ((it->getIPAddress() == UNSPECIFIED_ADDRESS && it->getPort() == port)
			|| (it->getIPAddress() == tempIPAddress && it->getPort() == port))
			return true;
	}
	return false;
}

bool Server::_tryBindSocket(Socket &socket, const std::string& IPAddress,
							unsigned short port) {
	for (int i = 0; i < 5; i++) {
		if (socket.binding(IPAddress, port))
			return true;
		_printError("socket.binding() to " + IPAddress + ":"
					+ toString(port) + " failed");
		ftSleep(500);
	}
	return false;
}

bool Server::_tryListenSocket(Socket &socket) {
	for (int i = 0; i < 5; i++) {
		if (socket.listening())
			return true;
		_printError("socket.listening() to " + socket.getIPAndPort() + " failed");
		ftSleep(500);
	}
	return false;
}

void Server::_check_revents_sockets(void) {
	for (size_t i = 0; i < _pollFd.size(); i++) {
		coloredLog("Checking revents for socket [" + toString(i) + "]", "", GREEN);
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
	int newSocket = accept(listener.fd, NULL, NULL);
	if (newSocket == -1) {
		_printError("server.listen accept() failed");
		return false;
	}
	struct pollfd newPoll;
	newPoll.fd = newSocket;
	newPoll.events = POLLIN;
	newPoll.revents = 0;
	_pollFd.push_back(newPoll);
	std::cout << "A new client is connected" << std::endl;
	return true;
}

ssize_t	Server::_read_persistent_connection(size_t client_index) {
	coloredLog("Request received :", "", BLUE);
	char buffer[4096];
	memset(buffer, 0, sizeof(buffer));
	std::string completeClientRequest;
	completeClientRequest = "";
	ssize_t bytesRead = (ssize_t)(sizeof(buffer));
	while (bytesRead >= (ssize_t)(sizeof(buffer))) {
		coloredLog("Reading request", "", YELLOW);
		bytesRead = read(_pollFd[client_index].fd, buffer, sizeof(buffer));
		coloredLog("Bytes read: ", toString(bytesRead), YELLOW);
		coloredLog("Request: ", buffer, PURPLE);
		coloredLog("End of request", "", YELLOW);
		if (bytesRead == -1)
			break;
		completeClientRequest.append(buffer, bytesRead);
	}
	if (bytesRead == 0)
	{
		std::cout << "A client socket has been close" << std::endl;
		close(_pollFd[client_index].fd);
		_pollFd.erase(_pollFd.begin() + client_index);
		return (bytesRead);
	}
	if (bytesRead != -1){
		HttpRequest httpRequest = HttpRequest(completeClientRequest);
		httpRequest.displayRequest();
		HttpResponse httpResponse;
		std::string response = httpResponse.getResponse((*this), httpRequest);
		write(_pollFd[client_index].fd, response.c_str(), response.size());
		coloredLog("Response sent", "[" + toString(client_index) + "] :", BLUE);
		coloredLog("Response: ", "\"" + response + "\"", PURPLE);
	}
	return bytesRead;
}

void Server::_printError(const std::string &error) {
	std::cerr << "webserver: " << error << " ("
			  << errno << ": " << strerror(errno) << ")" << std::endl;
}
