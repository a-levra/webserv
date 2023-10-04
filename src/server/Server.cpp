#include <iostream>
#include <unistd.h>

#include "server/Server.hpp"

Server::Server(void) {
	Socket	serverSocket = Socket();
	serverSocket.setReUse(true);
	serverSocket.binding(8000, "0.0.0.0");
	serverSocket.listening();
	_listenerSockets.push_back(serverSocket);
	_pollFd.push_back(serverSocket.getPollFd(POLLIN | POLLHUP));

	serverSocket = Socket();
	serverSocket.setReUse(true);
	serverSocket.binding(5000, "0.0.0.0");
	serverSocket.listening();
	_listenerSockets.push_back(serverSocket);
	_pollFd.push_back(serverSocket.getPollFd(POLLIN | POLLHUP));
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
		for (size_t i = 0; i < _pollFd.size(); ++i) {
			if (_pollFd[i].revents & POLLHUP) {
				std::cout << "A client socket has been close" << std::endl;
				close(_pollFd[i].fd);
				_pollFd.erase(_pollFd.begin() + i);
				i--;
				continue;
			}
			if (_pollFd[i].revents & POLLIN) {
				if (i  < _listenerSockets.size()) {
					int newSocket = accept(_pollFd[i].fd, NULL, NULL);
					if (newSocket == -1)
						throw std::runtime_error("Server listen: accept failed");
					struct pollfd newPoll;
					newPoll.fd = newSocket;
					newPoll.events = POLLIN;
					_pollFd.push_back(newPoll);
					std::cout << "A new client is connected" << std::endl;
				} else {
					// C'est un client existant, lisez les données du client et traitez-les
					char buffer[1024]; // Vous pouvez ajuster la taille du tampon selon vos besoins
					std::string completeClientRequest;
					while (true) {
						ssize_t bytesRead = read(_pollFd[i].fd, buffer, sizeof(buffer));
						if (bytesRead <= 0)
							break;
						completeClientRequest.append(buffer, bytesRead);
						if (bytesRead < sizeof(buffer))
							break;
					}
					std::cout << "A client has sent data" << std::endl;
				}
			}
		}
	}
}
