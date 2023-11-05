#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include "config/ConfigContext.hpp"
#include "config/ConfigLexer.hpp"

#include <string>
#include <vector>
#include <cstdlib>
#include <map>

#define AUTHORIZED_DIRECTIVES_IN_HTTP_CONTEXT "error_page index root \
	allow_methods client_max_body_size autoindex"
#define AUTHORIZED_DIRECTIVES_IN_SERVER_CONTEXT "error_page index root \
	allow_methods client_max_body_size autoindex listen server_name return"
#define AUTHORIZED_DIRECTIVES_IN_LOCATION_CONTEXT "error_page index root \
	allow_methods client_max_body_size autoindex return cgi_path"

class ConfigParser
{
public:

	enum codeError
	{
		NO_ERROR,
		FORBIDDEN_DIRECTIVE,
		FORBIDDEN_CONTEXT,
		DUPLICATE_CONTEXT,
		DUPLICATE_LOCATION,
		INVALID_ARGUMENTS,
		CONFLICTING_SERVER_NAME,
		NO_VIRTUAL_SERVER_FOUND,
	};

	struct virtualServer {
		std::string		ip;
		unsigned short	port;
		std::vector<std::string> serverName;
	};

	ConfigParser();
	ConfigParser(const ConfigParser &other);
	~ConfigParser();

	ConfigParser &operator=(const ConfigParser &other);

	bool parseConfigFile(const Context& mainContext);

	std::string getError() const;
	codeError	getCodeError() const;


private:

	bool	_parseMainContext(const Context& context);
	bool	_parseHttpContext(const Context& context);
	bool	_parseServersContext(const std::vector<Context>& servers);
	bool	_parseConflictingServerName();
	bool	_parseLocationsContext(const std::vector<Context>& locations);

	bool	_parseDirective(const std::string& name, const std::string& content);
	bool	_parseAllowSubContexts(const Context& context,
								   const std::vector<std::string>& allowSubContexts);
	bool	_parseDirectives(const Context& context, const std::vector<std::string>& allow_directives);
	bool	_parseIPAddress(const std::string& ipAddress);

	bool	_parseAllowMethods(const std::string& directiveContent);
	bool	_parseAutoIndex(const std::string& directiveContent);
	bool	_parseClientMaxBodySize(const std::string& directiveContent);
	bool	_parseErrorPage(const std::string& directiveContent);
	bool	_parseIndex(const std::string& directiveContent);
	bool	_parseListen(const std::string& directiveContent);
	bool	_parseRoot(const std::string& directiveContent);
	bool	_parseReturn(const std::string& directiveContent);
	bool	_parseServerName(const std::string& directiveContent);
	bool	_parseCGIPath(const std::string& directiveContent);
	bool	_parseAlias(const std::string &directiveContent);

	bool	_isEqualVirtualServersIP(const virtualServer& virtualServer1,
									 const virtualServer& virtualServer2);
  	bool	_hasSameServerName(const std::vector<std::string>&	serverName1,
							   const std::vector<std::string>&	serverName2);

	typedef bool (ConfigParser::*directiveFunction)(const std::string& directiveContent);
	typedef std::map<std::string, directiveFunction> directiveFunctionMap;

	directiveFunctionMap		_directiveFunctions;
	std::vector<virtualServer>	_parsedVirtualServers;
	codeError					_codeError;
	std::string					_error;

};

#endif
