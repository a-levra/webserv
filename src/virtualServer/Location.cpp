#include "virtualServer/Location.hpp"
#include "utils/utils.hpp"

Location::Location(void) {}

Location::Location(const Location &other) { *this = other; }

Location::~Location(void) {}

Location &Location::operator=(const Location &other) {
	if (this != &other) {
		this->_root = other._root;
		this->_index = other._index;
		this->_clientMaxBodySize = other._clientMaxBodySize;
		this->_autoindex = other._autoindex;
		this->_allowedMethods = other._allowedMethods;
	}
	return (*this);
}

std::string Location::getRoot() const {
	return (this->_root);
}

void Location::setRoot(const std::string &root) {
	this->_root = root;
}

std::string Location::getIndex() const {
	return (this->_index);
}

void Location::setIndex(const std::string &index) {
	this->_index = index;
}

size_t Location::getClientMaxBodySize() const {
	return (this->_clientMaxBodySize);
}

void Location::setClientMaxBodySize(size_t clientMaxBodySize) {
	this->_clientMaxBodySize = clientMaxBodySize;
}

bool Location::isAutoindex() const {
	return (this->_autoindex);
}

void Location::setAutoindex(bool autoindex) {
	this->_autoindex = autoindex;
}


std::vector<std::string> & Location::getMethods() {
	return (this->_allowedMethods);
}

void Location::addMethod(const std::string &method) {
	this->_allowedMethods.push_back(method);
}

void Location::display() {
	coloredLog("root: ", this->_root, YELLOW);
	coloredLog("index: ", this->_index, YELLOW);
	coloredLog("clientMaxBodySize: ", toString(this->_clientMaxBodySize), YELLOW);
	coloredLog("autoindex: ", toString(this->_autoindex), YELLOW);
	if (!this->_allowedMethods.empty() ){
		std::vector<std::string>::iterator it;
		for (it = this->_allowedMethods.begin(); it != this->_allowedMethods.end(); it++) {
			coloredLog("", *it, YELLOW);
		}
	}
	else
		coloredLog("methods: ", "none", YELLOW);
}

void Location::setUri(const char *string) {
	this->_uri = string;
}

std::string Location::getUri() {
	return (this->_uri);
}
