#include "virtualServer/Location.hpp"

location::location(void) {}

location::location(const location &other) { *this = other; }

location::~location(void) {}

location &location::operator=(const location &other) {
	if (this != &other) {
		this->_root = other._root;
		this->_index = other._index;
		this->_clientMaxBodySize = other._clientMaxBodySize;
		this->_autoindex = other._autoindex;
	}
	return (*this);
}

std::string location::getRoot() const {
	return (this->_root);
}

void location::setRoot(const std::string &root) {
	this->_root = root;
}

std::string location::getIndex() const {
	return (this->_index);
}

void location::setIndex(const std::string &index) {
	this->_index = index;
}

size_t location::getClientMaxBodySize() const {
	return (this->_clientMaxBodySize);
}

void location::setClientMaxBodySize(size_t clientMaxBodySize) {
	this->_clientMaxBodySize = clientMaxBodySize;
}

bool location::isAutoindex() const {
	return (this->_autoindex);
}

void location::setAutoindex(bool autoindex) {
	this->_autoindex = autoindex;
}


