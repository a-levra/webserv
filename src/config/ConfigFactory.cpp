#include "config/ConfigFactory.hpp"
#include "server/Socket.hpp"
#include "utils/utils.hpp"

#include <cstdlib>

ConfigFactory::ConfigFactory(void)
{

}

ConfigFactory::ConfigFactory(const ConfigFactory &other)
{
	*this = other;
}

ConfigFactory::~ConfigFactory(void)
{

}

ConfigFactory &ConfigFactory::operator=(const ConfigFactory &other)
{
	if (this == &other)
		return (*this);
	return (*this);
}

std::vector<VirtualServer> ConfigFactory::createVirtualServers(const Context &mainContext) {
	std::vector<VirtualServer>	virtualServers;
	const Context httpContext = mainContext.getConstSubContexts()[0];
	const std::vector<Context>& servers = httpContext.getConstSubContexts();

	std::vector<Context>::const_iterator it;
	for (it = servers.begin(); it != servers.end(); it++) {
		virtualServers.push_back(_createVirtualServer(*it));
	}
	return virtualServers;
}

VirtualServer ConfigFactory::_createVirtualServer(const Context& serverContext) {
	VirtualServer	server;
	const std::vector<Context>& locations = serverContext.getConstSubContexts();
	const std::map<std::string, std::string>& directives = serverContext.getDirectives();
	std::map<std::string, std::string>::const_iterator listenIt = directives.find("listen");

	if (listenIt != directives.end()) {
		std::vector<std::string>	ipAddress = splitDelimiter(listenIt->second, ':');
		if (ipAddress[0] == LOCALHOST)
			server.setIPAddress(LOOPBACK_IP);
		else
			server.setIPAddress(ipAddress[0]);
		server.setPort(std::atoi(ipAddress[1].c_str()));
		server.setServerName(_getServerNamesFromDirective(directives));
	}
	std::vector<Context>::const_iterator it;
	for (it = locations.begin(); it != locations.end(); it++) {
		Location location = _createLocation(*it);
		server.addLocation(location.getURI(), location);
	}
	return server;
}

Location ConfigFactory::_createLocation(const Context &locationContext) {
	Location	location(locationContext.getArguments());

	const std::map<std::string, std::string>& directives = locationContext.getDirectives();
	std::map<std::string, std::string>::const_iterator it;

	for(it = directives.begin(); it != directives.end(); it++) {
		location.addDirective(it->first, it->second);
	}
	return location;
}

std::vector<std::string> ConfigFactory::_getServerNamesFromDirective(const std::map<std::string, std::string> &directives) {
	std::map<std::string, std::string>::const_iterator it = directives.find("server_name");
	std::string serverNamesNotSplitted;
	if (it == directives.end())
		serverNamesNotSplitted = "";
	else
		serverNamesNotSplitted = it->second;
	return (splitWhiteSpace(serverNamesNotSplitted));
}

