#include "server/Client.hpp"
#include "server/Socket.hpp"

#include <iostream>
#include <ctime>
#include "unistd.h"

Client::Client() {

}

Client::Client(int fd, struct sockaddr_in address,
			   struct sockaddr_in entryAddress) {
	_address = address;
	_entryAddress = entryAddress;
	_IPAddress = Socket::networkToStr(address.sin_addr.s_addr);
	_entryIPAddress = Socket::networkToStr(entryAddress.sin_addr.s_addr);
	_lastActivity = std::time(NULL);
	_fd = fd;
	_port = ntohs(address.sin_port);
	_entryPort = ntohs(entryAddress.sin_port);
}

Client::Client(const Client &other) {
	*this = other;
}

Client::~Client() {

}

Client &Client::operator=(const Client &other)
{
	if (this == &other)
		return *this;
	_request = other._request;
	_address = other._address;
	_entryAddress = other._entryAddress;
	_IPAddress = other._IPAddress;
	_entryIPAddress = other._entryIPAddress;
	_lastActivity = other._lastActivity;
	_fd = other._fd;
	_port = other._port;
	_entryPort = other._entryPort;
	return *this;
}

void Client::appendRawRequest(const std::string &rawRequest) {
	_lastActivity = std::time(NULL);
//	_request.appendRawRequest(rawRequest);
	_rawRequest.append(rawRequest);
}

enum REQUEST_VALIDITY Client::checkRequestValidity() {
	_request = HttpRequest(_rawRequest);
	_request.parse();
	return _request.checkValidity();
}

HttpRequest Client::getRequest() {
	_rawRequest = "";
	return _request;
}

int Client::getFD() {
	return _fd;
}

time_t Client::getMSSinceLastActivity() {
	return ((std::time(NULL) - _lastActivity) * 1000);
}

int Client::disconnect() const {
	return (close(_fd));
}
