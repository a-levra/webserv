#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <cerrno>
#include <cstdlib>
#include <string>

#include "server/Socket.hpp"
#include "utils/utils.hpp"

Socket::Socket() { }

Socket::Socket(const Socket &other) {
	*this = other;
}

Socket::~Socket(void) { }

Socket &Socket::operator=(const Socket &other) {
	if (this == &other)
		return *this;
	_fd = other._fd;
	_address = other._address;
	_IPAddress = other._IPAddress;
	_port = other._port;
	return *this;
}

bool Socket::initialize() {
	this->_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_fd == -1)
		return false;
	std::memset(&this->_address, 0, sizeof(this->_address));
	return true;
}

bool Socket::binding(const std::string& IPAddress, const unsigned short port) {
	if (IPAddress == LOCALHOST)
		_IPAddress = LOOPBACK_IP;
	else
		_IPAddress = IPAddress;
	_port = port;
	_address.sin_family = AF_INET;
	_address.sin_port = htons(port);
	if (!_calculateRawIPAddress())
		return false;
	_address.sin_addr.s_addr = htonl(_rawIPAddress);
	if (bind(_fd, (struct sockaddr *)&_address, sizeof(_address)) == -1) {
		return false;
	}
	return true;
}

bool Socket::listening() const {
	if (listen(_fd, SOMAXCONN) == -1)
		return false;
	return true;
}

void	Socket::closeFD() {
	close(_fd);
}

int Socket::getFD() const {
	return _fd;
}

unsigned short Socket::getPort() const {
	return _port;
}

std::string Socket::getIPAddress() const {
	return _IPAddress;
}

std::string Socket::getIPAndPort() const {
	return _IPAddress + ":" + toString(_port);
}

struct pollfd Socket::getPollFd(const short events) const {
	return ((struct pollfd){.fd = _fd, .events = events, .revents = 0});
}

bool	Socket::_calculateRawIPAddress()
{
	uint32_t result = 0;
	unsigned int shift = 24;
	std::string tmpIPAddress = _IPAddress;
	char* endPtr;

	_rawIPAddress = 0;
	for (int i = 0; i < 4; i++) {
		std::string str_byte = tmpIPAddress.substr(0, tmpIPAddress.find('.'));
		if (str_byte.length() == 0)
			return false;
		long byte = std::strtol(str_byte.c_str(), &endPtr, 10);
		if (errno == ERANGE || *endPtr != '\0' ||byte > 255)
			return false;
		result |= byte << shift;
		shift -= 8;
		size_t pos = tmpIPAddress.find('.');
		if (i == 3 && pos != std::string::npos)
			return false;
		tmpIPAddress = tmpIPAddress.substr(tmpIPAddress.find('.') + 1);
	}
	_rawIPAddress = result;
	return true;
}
