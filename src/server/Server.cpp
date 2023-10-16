#include <iostream>
#include <unistd.h>
#include <cstring>

#include "server/Server.hpp"
#include "HttpMessages/AHttpMessage.hpp"
#include "HttpMessages/HttpRequest.hpp"
#include "HttpMessages/HttpResponse.hpp"
#include "utils/utils.hpp"

Server::Server() {}

Server::Server(const std::vector<VirtualServer>& virtualServers) {
	std::vector<VirtualServer>::const_iterator it;
	for (it = virtualServers.begin(); it != virtualServers.end(); it++) {
		Socket	serverSocket = Socket();
		serverSocket.setReUse(true);
		serverSocket.binding(it->getIP(), it->getPort());
		serverSocket.listening();
		_listenerSockets.push_back(serverSocket);
		_pollFd.push_back(serverSocket.getPollFd(POLLIN | POLLHUP));
	}
}

Server::Server(const Server &other) { *this = other; }

Server::~Server(void) {}

Server &Server::operator=(const Server &other) {
	if (this == &other)
		return *this;
	_listenerSockets = other._listenerSockets;
	_pollFd = other._pollFd;
	_virtualServers = other._virtualServers;
	return (*this);
}

void Server::listen(void) {
	coloredLog("Start listenning...", "", GREEN);
	for (size_t i = 0; i < _listenerSockets.size(); i++) {
		std::cout << _listenerSockets[i].getIP() << ":" << _listenerSockets[i].getPort() << std::endl;
	}
	while (true) {
		if (poll(_pollFd.data(), _pollFd.size(), -1) == -1)
			throw std::runtime_error("Server listen: poll failed");
		_check_revents_sockets();
	}
}

void Server::_check_revents_sockets(void) {
	for (size_t i = 0; i < _pollFd.size(); i++) {
		if ((_pollFd[i].revents & POLLIN) == 0)
			continue;
		if (i  < _listenerSockets.size()) {
			_accept_new_client(_pollFd[i]);
		} else if (_read_persistent_connection(i) == 0) {
			i--;
		}
	}
}

void Server::_accept_new_client(struct pollfd listener) {
	int newSocket = accept(listener.fd, NULL, NULL);
	if (newSocket == -1)
		throw std::runtime_error("Server listen: accept failed");
	struct pollfd newPoll;
	newPoll.fd = newSocket;
	newPoll.events = POLLIN;
	newPoll.revents = 0;
	_pollFd.push_back(newPoll);
	std::cout << "A new client is connected" << std::endl;
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
		coloredLog("Response sent", "[" + toString(client_index) + "]", BLUE);
	}
	std::cout << "A client has sent data" << std::endl;

	return bytesRead;
}


void Server::displayVirtualServers() {
	coloredLog( "Webserv virtual servers(hosts): ", "", BLUE);
	for (size_t i = 0; i < _virtualServers.size(); i++) {
		coloredLog("\thost [" + toString(i) + "]: ", _virtualServers[i].getServerName()[0], PURPLE);
	}
}

VirtualServer *Server::getVirtualServer(const std::string &serverName) {
	for (size_t i = 0; i < _virtualServers.size(); i++) {
		if (_virtualServers[i].getServerName()[0] == serverName) //todo : ne check que le premier serverName..
			return &_virtualServers[i];
	}
	return NULL;
}
