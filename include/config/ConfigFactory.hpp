#ifndef CONFIGFACTORY_HPP
# define CONFIGFACTORY_HPP

# include "virtualServer/VirtualServer.hpp"
# include "virtualServer/Location.hpp"
# include "config/ConfigContext.hpp"

# include <vector>

class ConfigFactory
{
public:
	ConfigFactory(void);
	ConfigFactory(const ConfigFactory &other);
	~ConfigFactory(void);

	ConfigFactory &operator=(const ConfigFactory &other);

	static std::vector<VirtualServer>	createVirtualServers(const Context& mainContext);

private:
	static VirtualServer			_createVirtualServer(const Context& serverContext);
	static Location					_createLocation(const Context& locationContext);
	static std::vector<std::string> _getServerNamesFromDirective(const std::map<std::string, std::string> &directives);
};

#endif
