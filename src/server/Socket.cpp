#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <cstdio>

#include "server/Socket.hpp"

Socket::Socket(void) {
	//	TODO: secure
	this->_fd = socket(AF_INET6, SOCK_STREAM, 0);
	if (this->_fd == -1)
		perror("Error");
	std::memset(&this->_address, 0, sizeof(this->_address));
}


Socket::Socket(const Socket &other) {
	*this = other;
}

Socket::~Socket(void) { }

Socket &Socket::operator=(const Socket &other) {
	if (this == &other)
		return *this;
	this->_fd = other._fd;
	this->_address = other._address;
	return *this;
}

void Socket::binding(const sa_family_t family, const int port,
					 const char* ip_address) {
	_address.sin6_family = family;
	_address.sin6_port = htons(port);
	inet_pton(family, ip_address, &_address.sin6_addr);
	//	TODO: secure
	if (bind(_fd, (struct sockaddr *)&_address, sizeof(_address)) == -1)
		perror("Error");
}

void	Socket::listening(void) {
	//	TODO: secure
	listen(_fd, SOMAXCONN);
}

void	Socket::closeFd() {
	close(_fd);
}

int Socket::getFd(void) const {
	return _fd;
}


struct pollfd Socket::getPollFd(const short events) const {
	return ((struct pollfd){.fd = _fd, .events = events});
}

void	Socket::setReUse(const int option) {
	int reuse = option;
	//	TODO: secure
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
}
