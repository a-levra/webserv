#include "virtualServer/VirtualServer.hpp"
#include "virtualServer/Location.hpp"
#include "utils/utils.hpp"

VirtualServer::VirtualServer(void) {}

VirtualServer::VirtualServer(const VirtualServer &other) { *this = other; }

VirtualServer::~VirtualServer(void) {}

VirtualServer &VirtualServer::operator=(const VirtualServer &other) {
	if (this != &other) {
		setLocations(other._locations);
		this->_ip = other._ip;
		this->_name = other._name;
		this->_port = other._port;
	}
	return (*this);
}

void VirtualServer::setName(const std::string &name) {
	_name = name;
}

void VirtualServer::setIP(const std::string &ip) {
	_ip = ip;
}

void VirtualServer::setPort(const short port) {
	_port = port;
}

void VirtualServer::setLocations(const std::map<std::string, Location> &locations) {
	_locations = locations;
}

const std::string & VirtualServer::getName() const {
	return _name;
}

std::string VirtualServer::getIP() const {
	return _ip;
}

short VirtualServer::getPort() const {
	return _port;
}

std::map<std::string, Location> VirtualServer::getLocations() const {
	return _locations;
}

void VirtualServer::addLocation(Location &location) {
	_locations.insert(std::pair<std::string, ::Location>(location.getUri(), location));
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
	coloredLog("Virtual server " + this->_name + ": ", "", PURPLE);
	coloredLog("\tname: ", _name, GREEN);
	coloredLog("\tip: ", _ip, GREEN);
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

