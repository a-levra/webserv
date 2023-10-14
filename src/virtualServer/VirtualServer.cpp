#include "virtualServer/VirtualServer.hpp"
#include "virtualServer/Location.hpp"
virtualServer::virtualServer(void): _ipAddress("localhost"), _port(80) {
	_serverName.push_back("");
}

virtualServer::virtualServer(const virtualServer &other) { *this = other; }

virtualServer::~virtualServer(void) {}

virtualServer &virtualServer::operator=(const virtualServer &other) {
	if (this != &other) {
		this->_ipAddress = other._ipAddress;
		this->_port = other._port;
		this->_serverName = other._serverName;
		setLocations(other._locations);
	}
	return (*this);
}

void virtualServer::setServerName(const std::vector<std::string>& serverName) {
	_serverName = serverName;
}

void virtualServer::setIP(const std::string &ip) {
	_ipAddress = ip;
}

void virtualServer::setPort(const int port) {
	_port = port;
}

void virtualServer::setLocations(const std::map<std::string, Location> &locations) {
	_locations = locations;
}

const std::vector<std::string>& virtualServer::getServerName() const {
	return _serverName;
}

std::string virtualServer::getIP() const {
	return _ipAddress;
}

short virtualServer::getPort() const {
	return _port;
}

std::map<std::string, Location> virtualServer::getLocations() const {
	return _locations;
}

void virtualServer::addLocation(const std::string &name, const Location &location) {
	_locations.insert(std::pair<std::string, ::Location>(name, location));
}

void virtualServer::removeLocation(const std::string &name) {
	_locations.erase(name);
}

//example: name = /index.html
Location virtualServer::getLocation(const std::string &name) const {
	return _locations.find(name)->second;
}

Location virtualServer::getLocationByUri(const std::string &uri) const {
	std::map<std::string, Location>::const_iterator it;
	for (it = _locations.begin(); it != _locations.end(); it++) {
		if (uri.find(it->first) != std::string::npos)
			return it->second;
	}
	return _locations.find("/")->second;
}
