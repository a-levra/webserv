#include "server/Client.hpp"
#include "server/Socket.hpp"

#include <iostream>
#include <ctime>
#include "unistd.h"

Client::Client() {

}

Client::Client(int fd, struct sockaddr_in address,
			   struct sockaddr_in entryAddress) {
	_socket = Socket(fd, address);
	_entryAddress = entryAddress;
	_entryIPAddress = Socket::networkToStr(entryAddress.sin_addr.s_addr);
	_lastActivity = std::time(NULL);
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
	_socket = other._socket;
	_entryAddress = other._entryAddress;
	_entryIPAddress = other._entryIPAddress;
	_lastActivity = other._lastActivity;
	_entryPort = other._entryPort;
	return *this;
}

enum HttpRequest::REQUEST_VALIDITY Client::checkRequestValidity() {
	_request = HttpRequest(_rawRequest);
	return _request.checkValidity();
}

void Client::appendRawRequest(const std::string &rawRequest) {
	_rawRequest.append(rawRequest);
}

void Client::setRawRequest(const std::string &rawRequest) {
	_rawRequest = rawRequest;
}

HttpRequest Client::getRequest() {
	return _request;
}

int Client::getFD() {
	return _socket.getFD();
}

const std::string &Client::getEntryIPAddress() const {
	return _entryIPAddress;
}

unsigned short Client::getEntryPort() const {
	return _entryPort;
}

time_t Client::getMSSinceLastActivity() {
	return ((std::time(NULL) - _lastActivity) * 1000);
}

time_t Client::updateLastActivity() {
	_lastActivity = std::time(NULL);
	return _lastActivity;
}

int Client::disconnect() const {
	return _socket.disconnect();
}
