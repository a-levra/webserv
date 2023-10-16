#include "virtualServer/VirtualServer.hpp"
#include "virtualServer/Location.hpp"
#include "utils/utils.hpp"

VirtualServer::VirtualServer(void): _ipAddress("localhost"), _port(80) {
	_serverName.push_back("");
}

VirtualServer::VirtualServer(const VirtualServer &other) { *this = other; }

VirtualServer::~VirtualServer(void) {}

VirtualServer &VirtualServer::operator=(const VirtualServer &other) {
	if (this != &other) {
		this->_ipAddress = other._ipAddress;
		this->_port = other._port;
		this->_serverName = other._serverName;
		setLocations(other._locations);
	}
	return (*this);
}

void VirtualServer::setServerName(const std::vector<std::string>& serverName) {
	_serverName = serverName;
}

void VirtualServer::setIP(const std::string &ip) {
	_ipAddress = ip;
}

void VirtualServer::setPort(const int port) {
	_port = port;
}

void VirtualServer::setLocations(const std::map<std::string, Location> &locations) {
	_locations = locations;
}

const std::vector<std::string>& VirtualServer::getServerName() const {
	return _serverName;
}

std::string VirtualServer::getIP() const {
	return _ipAddress;
}

short VirtualServer::getPort() const {
	return _port;
}

std::map<std::string, Location> VirtualServer::getLocations() const {
	return _locations;
}

void VirtualServer::addLocation(const std::string &name, const Location &location) {
	_locations.insert(std::pair<std::string, ::Location>(name, location));
}

void VirtualServer::removeLocation(const std::string &name) {
	_locations.erase(name);
}

//example: name = /index.html
Location * VirtualServer::getLocation(const std::string &name) {
	std::map<std::string, Location>::iterator res = _locations.find(name);
	if (res == _locations.end())
		return (NULL);
	return &(res->second);
}
void VirtualServer::display() {
	coloredLog("Virtual server : ", "", PURPLE);
	coloredLog("\tname: ", _serverName[0], GREEN);
	coloredLog("\tip: ", _ipAddress, GREEN);
	coloredLog("\tport: ", toString(_port), GREEN);
	coloredLog("\tlocation : ", "", GREEN);
	if (_locations.empty()) {
		coloredLog("", "(empty)", GREEN);
		return;
	}
	std::map<std::string, Location>::iterator it;
	for (it = _locations.begin(); it != _locations.end(); it++)
		printBold("\t\t" + it->first);
}

