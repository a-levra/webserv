#include <iostream>
#include <unistd.h>

#include "server/Server.hpp"

Server::Server(const std::vector<virtualServer>& virtualServers) {
	std::vector<virtualServer>::const_iterator it;
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
	return (*this);
}

void Server::listen(void) {
	std::cout << "Start listenning..." << std::endl;
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
	char buffer[1024]; // Vous pouvez ajuster la taille du tampon selon vos besoins
	std::string completeClientRequest;
	ssize_t bytesRead = read(_pollFd[client_index].fd, buffer, sizeof(buffer));
	while (bytesRead > 0 ) {
		completeClientRequest.append(buffer, bytesRead);
		bytesRead = read(_pollFd[client_index].fd, buffer, sizeof(buffer));
	}
	if (bytesRead == 0)
	{
		std::cout << "A client socket has been close" << std::endl;
		close(_pollFd[client_index].fd);
		_pollFd.erase(_pollFd.begin() + client_index);
		return (bytesRead);
	}
	std::cout << "A client has sent data" << std::endl;
	return bytesRead;
}