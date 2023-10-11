#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include "config/ConfigContext.hpp"

#include <string>
#include <vector>
#include <cstdlib>
#include <map>

class ConfigParser
{
public:

	enum codeError
	{
		NO_ERROR,
		FORBIDDEN_DIRECTIVE,
		FORBIDDEN_CONTEXT,
		DUPLICATE_CONTEXT,
		INVALID_ARGUMENTS,
	};


	ConfigParser();
	ConfigParser(const ConfigParser &other);
	~ConfigParser();

	ConfigParser &operator=(const ConfigParser &other);

	int	parseConfigFile(const std::string& fileName);


private:

	bool	_parseMainContext(const Context& context);
	bool	_parseHttpContext(const Context& context);
	bool	_parseServersContext(const std::vector<Context>& servers);
	bool	_parseLocationsContext(const std::vector<Context>& locations);

	// Utils
	bool	_parseDirective(const std::string& name, const std::string& content);
	bool	_checkSubContext(const Context& context,
							 const std::vector<std::string>& contexts);
	bool	_checkDirectives(const Context& context, const std::vector<std::string>& allow_directives);
	bool	_parseIPAddress(const std::string& ipAddress);

	// Directive

	bool	_parseAllowMethods(const std::string& directiveContent);
	bool	_parseAutoIndex(const std::string& directiveContent);
	bool	_parseClientMaxBodySize(const std::string& directiveContent);
	bool	_parseErrorPage(const std::string& directiveContent);
	bool	_parseIndex(const std::string& directiveContent);
	bool	_parseListen(const std::string& directiveContent);
	bool	_parseRoot(const std::string& directiveContent);
	bool	_parseReturn(const std::string& directiveContent);
	bool	_parseServerName(const std::string& directiveContent);

	codeError	_codeError;
	std::string	_error;
};



#endif
