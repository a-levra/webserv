#include "Server.hpp"

Server::Server(void) {}

Server::Server(const Server &other) { *this = other; }

Server::~Server(void) {}

Server &Server::operator=(const Server &other) {
	if (this != &other) {

	}
	return (*this);
}