#include <cstring>

#include "virtualServer/VirtualServer.hpp"
#include "virtualServer/Location.hpp"
#include "utils/utils.hpp"
#include "logger/logging.hpp"

VirtualServer::VirtualServer(): _ipAddress(DEFAULT_VIRTUAL_SERVER_IP_ADDRESS),
								_port(DEFAULT_VIRTUAL_SERVER_PORT) {
	_serverName.push_back(DEFAULT_VIRTUAL_SERVER_NAME);
}

VirtualServer::VirtualServer(const VirtualServer &other) { *this = other; }

VirtualServer::~VirtualServer() {}

VirtualServer &VirtualServer::operator=(const VirtualServer &other) {
	if (this == &other)
		return *this;
	_ipAddress = other._ipAddress;
	_port = other._port;
	_serverName = other._serverName;
	setLocations(other._locations);
	return *this;
}

void VirtualServer::addLocation(const std::string &name, const Location &location) {
	_locations.insert(std::pair<std::string, ::Location>(name, location));
}

void VirtualServer::removeLocation(const std::string &name) {
	_locations.erase(name);
}

std::map<std::string, Location> VirtualServer::getLocations() const {
	return _locations;
}

std::vector<std::string> & VirtualServer::getServerName() {
	if (_serverName.empty())
		_serverName.push_back("");
	return _serverName;
}

std::string VirtualServer::getIPAddress() const {
	return _ipAddress;
}

unsigned short VirtualServer::getPort() const {
	return _port;
}

std::string VirtualServer::getIPAndPort() const {
	return  _ipAddress + ":" + toString(_port);
}

//example: name = /index.html

Location * VirtualServer::getLocation(const std::string &URI) {
	std::map<std::string, Location>::iterator res = _locations.find(URI);
	if (res != _locations.end())
		return &(res->second);

	logging::debug(B_YELLOW "Searching best match for \"" + URI + "\"" THIN);
	logging::debug(B_YELLOW "List of scores : " THIN);
	size_t score = 0;
	size_t bestScore = 0;
	Location *bestLocation = NULL;
	for (res = _locations.begin(); res != _locations.end(); res++) {
		score = (strncmp(res->first.c_str(), URI.c_str(), (size_t)res->first.length()) == 0 ? (size_t)res->first.length():0);
		logging::debug(B_YELLOW "\t\"" + res->first + "\" : " + toString(score) + THIN);
		if (score > bestScore){
			bestScore = score;
			bestLocation = 	&(res->second);
		}
	}
	return bestLocation;
}

void VirtualServer::setServerName(const std::vector<std::string>& serverName) {
	_serverName = serverName;
}

void VirtualServer::setIPAddress(const std::string &ip) {
	_ipAddress = ip;
}

void VirtualServer::setPort(const int port) {
	_port = port;
}

void VirtualServer::setLocations(const std::map<std::string, Location> &locations) {
	_locations = locations;
}

void VirtualServer::display() {
	logging::debug(B_BLUE "Virtual server : ");
	logging::debug(B_BLUE "\tname: " THIN + _serverName[0]);
	logging::debug(B_BLUE "\tip: " THIN + _ipAddress);
	logging::debug(B_BLUE "\tport: " THIN + toString(_port));
	logging::debug(B_BLUE "\tlocation : " THIN );
	if (_locations.empty()) {
		logging::debug("(empty)" COLOR_RESET );
		return;
	}
	std::map<std::string, Location>::iterator it;
	for (it = _locations.begin(); it != _locations.end(); it++)
		printBold("\t\t" + it->first);
}

