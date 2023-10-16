#include <iostream>
#include <unistd.h>
#include <cstring>

#include "server/Server.hpp"
#include "HttpMessages/AHttpMessage.hpp"
#include "HttpMessages/HttpRequest.hpp"
#include "HttpMessages/HttpResponse.hpp"
#include "utils/utils.hpp"


Server::Server(bool localMode) : _localMode(localMode) {
	std::string ipToBind = localMode ? "localhost" : "0.0.0.0";
	Socket serverSocket = Socket();
	serverSocket.setReUse(true);
	serverSocket.binding(ipToBind, 5000);
	serverSocket.listening();
	_listenerSockets.push_back(serverSocket);
	_pollFd.push_back(serverSocket.getPollFd(POLLIN | POLLHUP));

	serverSocket = Socket();
	serverSocket.setReUse(true);
	serverSocket.binding(ipToBind, 8000);
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
	coloredLog("Start listenning...", "", GREEN);
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
				if (i < _listenerSockets.size()) {
					int newSocket = accept(_pollFd[i].fd, NULL, NULL);
					if (newSocket == -1)
						throw std::runtime_error("Server listen: accept failed");
					struct pollfd newPoll;
					newPoll.fd = newSocket;
					newPoll.events = POLLIN;
					_pollFd.push_back(newPoll);
					coloredLog("A new client is connected ", "", GREEN);
				} else {
					coloredLog("Request received :", "[" + toString(i) + "]", BLUE);
					char buffer[4096];
					memset(buffer, 0, sizeof(buffer));
					std::string completeClientRequest;
					completeClientRequest = "";
					ssize_t bytesRead = (ssize_t)(sizeof(buffer));
					while (bytesRead >= (ssize_t)(sizeof(buffer))) {
						coloredLog("Reading request", "[" + toString(i) + "]", YELLOW);
						bytesRead = read(_pollFd[i].fd, buffer, sizeof(buffer));
						coloredLog("Bytes read: ", toString(bytesRead), YELLOW);
						coloredLog("Request: ", buffer, PURPLE);
						coloredLog("End of request", "[" + toString(i) + "]", YELLOW);
						if (bytesRead == -1)
							break;
						completeClientRequest.append(buffer, bytesRead);
					}
					if (bytesRead != -1){
						HttpRequest httpRequest = HttpRequest(completeClientRequest);
						httpRequest.displayRequest();
						HttpResponse httpResponse;
						std::string response = httpResponse.getResponse((*this), httpRequest);
						write(_pollFd[i].fd, response.c_str(), response.size());
						coloredLog("Response sent", "[" + toString(i) + "]", BLUE);
					}
					coloredLog("Closing connection", "[" + toString(i) + "]", BLUE);
					close(_pollFd[i].fd);
				}
			}
		}
	}
}

void Server::addVirtualServer(const VirtualServer &virtualServer) {
	_virtualServers.push_back(virtualServer);
}

Location * Server::getLocation(const std::string &uri, VirtualServer &host) {
	return host.getLocation(uri);
}

VirtualServer * Server::getVirtualServer(const std::string &serverName) {
	for (size_t i = 0; i < _virtualServers.size(); i++) {
		if (_virtualServers[i].getName() == serverName)
			return &(_virtualServers[i]);
	}
	return(NULL);

}

void Server::displayVirtualServers() {
	coloredLog( "Webserv virtual servers(hosts): ", "", BLUE);
	for (size_t i = 0; i < _virtualServers.size(); i++) {
		coloredLog("\thost [" + toString(i) + "]: ", _virtualServers[i].getName(), PURPLE);
	}
}
