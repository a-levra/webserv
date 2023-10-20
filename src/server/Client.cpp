#include "server/Client.hpp"

#include <iostream>
#include <ctime>

Client::Client() {

}

Client::Client(int fd, struct sockaddr_in address,
			   struct sockaddr_in entryAddress) {
	_address = address;
	_entryAddress = entryAddress;
	_IPAddress = _rawIPAddressToStrIPAddress(address.sin_addr.s_addr);
	_entryIPAddress = _rawIPAddressToStrIPAddress(entryAddress.sin_addr.s_addr);
	_lastActivity = std::time(NULL);
	_fd = fd;
	_port = _rawPortToPort(address.sin_port);
	_entryPort = _rawPortToPort(entryAddress.sin_port);
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
	this->_request = other._request;
	this->_address = other._address;
	this->_entryAddress = other._entryAddress;
	this->_IPAddress = other._IPAddress;
	this->_entryIPAddress = other._entryIPAddress;
	this->_lastActivity = other._lastActivity;
	this->_fd = other._fd;
	this->_port = other._port;
	this->_entryPort = other._entryPort;
	std::cout << "IPAddress: " << _IPAddress << ":" << _port << std::endl;
	std::cout << "EntryIPAddress: " << _entryIPAddress << ":" << _entryPort << std::endl;
	return *this;
}

void Client::appendRawRequest(const std::string &rawRequest) {
	_lastActivity = std::time(NULL);
	std::cout << _lastActivity << std::endl;
	_request.appendRawRequest(rawRequest);
}

enum REQUEST_VALIDITY Client::checkRequestValidity() {
	std::string tmp = _request.getRawRequest();
	_request.parse();
	return _request.checkValidity();
}

HttpRequest Client::getRequest() {
	return _request;
}

int Client::getFD() {
	return _fd;
}

time_t Client::getMSSinceLastActivity() {
	std::cout << (std::time(NULL) - _lastActivity) * 1000 << std::endl;
	return ((std::time(NULL) - _lastActivity) * 1000);
}

std::string Client::_rawIPAddressToStrIPAddress(uint32_t rawIPAddress) {
	rawIPAddress = ntohl(rawIPAddress);
	std::string	IPAddress;
	unsigned int shift = 24;

	for (int i = 0; i < 4; i++) {
		int byte = (rawIPAddress >> shift) & 0xFF;
		if (i == 0)
			IPAddress += toString(byte);
		else
			IPAddress += "." + toString(byte);
		shift -= 8;
	}
	return IPAddress;
}

unsigned short Client::_rawPortToPort(uint16_t rawPort) {
	return ntohs(rawPort);
}