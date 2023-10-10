#include "virtualServer/VirtualServer.hpp"
#include "virtualServer/Location.hpp"
virtualServer::virtualServer(void) {}

virtualServer::virtualServer(const virtualServer &other) { *this = other; }

virtualServer::~virtualServer(void) {}

virtualServer &virtualServer::operator=(const virtualServer &other) {
	if (this != &other) {
		setLocations(other._locations);
		this->_ip = other._ip;
		this->_name = other._name;
		this->_port = other._port;
	}
	return (*this);
}

void virtualServer::setName(const std::string &name) {
	_name = name;
}

void virtualServer::setIP(const std::string &ip) {
	_ip = ip;
}

void virtualServer::setPort(const short port) {
	_port = port;
}

void virtualServer::setLocations(const std::map<std::string, location> &locations) {
	_locations = locations;
}

std::string virtualServer::getName() const {
	return _name;
}

std::string virtualServer::getIP() const {
	return _ip;
}

short virtualServer::getPort() const {
	return _port;
}

std::map<std::string, location> virtualServer::getLocations() const {
	return _locations;
}

void virtualServer::addLocation(const std::string &name, const location &location) {
	_locations.insert(std::pair<std::string, ::location>(name, location));
}

void virtualServer::removeLocation(const std::string &name) {
	_locations.erase(name);
}

//example: name = /index.html
location virtualServer::getLocation(const std::string &name) const {
	return _locations.find(name)->second;
}

location virtualServer::getLocationByUri(const std::string &uri) const {
	std::map<std::string, location>::const_iterator it;
	for (it = _locations.begin(); it != _locations.end(); it++) {
		if (uri.find(it->first) != std::string::npos)
			return it->second;
	}
	return _locations.find("/")->second;
}
