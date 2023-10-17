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

static bool	isDigitStr(const std::string& str);

Socket::Socket(void) {
	this->_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_fd == -1)
		throw std::runtime_error("Socket: socket failed");
	std::memset(&this->_address, 0, sizeof(this->_address));
}

Socket::Socket(const std::string &ip_address, int port) {
	this->_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_fd == -1)
		throw std::runtime_error("Socket: socket failed");
	std::memset(&this->_address, 0, sizeof(this->_address));
	binding(ip_address, port);
}

Socket::Socket(const Socket &other) {
	*this = other;
}

Socket::~Socket(void) { }

Socket &Socket::operator=(const Socket &other) {
	if (this == &other)
		return *this;
	_fd = other._fd;
	_address = other._address;
	_ip_adress = other._ip_adress;
	_port = other._port;
	return *this;
}

void Socket::binding(const std::string& ip_address, const int port) {
	_port = port;
	_ip_adress = std::string(ip_address);
	_address.sin_family = AF_INET;
	_address.sin_port = htons(port);
	_address.sin_addr.s_addr = htonl(_convertIPToBinary(ip_address));
	if (bind(_fd, (struct sockaddr *)&_address, sizeof(_address)) == -1)
		throw std::runtime_error("Socket: bind failed");
}

void	Socket::listening(void) {
	if (listen(_fd, SOMAXCONN) == -1)
		throw std::runtime_error("Socket: listen failed");
}

void	Socket::closeFD() {
	close(_fd);
}

int Socket::getFD(void) const {
	return _fd;
}

int Socket::getPort() const {
	return _port;
}

std::string Socket::getIP() const {
	return _ip_adress;
}

struct pollfd Socket::getPollFd(const short events) const {
	return ((struct pollfd){.fd = _fd, .events = events, .revents = 0});
}

uint32_t	Socket::_convertIPToBinary(std::string ip_address)
{
	uint32_t result = 0;
	unsigned int shift = 24;

	if (ip_address == "localhost")
		return INADDR_LOOPBACK;
	for (int i = 0; i < 4; i++) {
		std::string str_byte = ip_address.substr(0, ip_address.find('.'));
		if (str_byte.length() == 0 || !isDigitStr(str_byte))
			throw std::runtime_error("_convertIPToBinary() invalid ip");
		unsigned int byte = std::atoi(str_byte.c_str());
		if (errno == ERANGE || byte > 255)
			throw std::runtime_error("_convertIPToBinary() invalid ip");
		result |= byte << shift;
		shift -= 8;
		size_t pos = ip_address.find('.');
		if (i == 3 && pos != std::string::npos)
			throw std::runtime_error("_convertIPToBinary() invalid ip");
		ip_address = ip_address.substr(ip_address.find('.') + 1);
	}
	return result;
}

static bool	isDigitStr(const std::string& str)
{
	for (size_t i = 0; i < str.size(); i++) {
		if (!std::isdigit(str[i]))
			return false;
	}
	return true;
}