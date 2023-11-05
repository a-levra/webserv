#include "config/ConfigParser.hpp"

#include <limits>
#include <algorithm>
#include <cerrno>

#include "virtualServer/VirtualServer.hpp"
#include "config/ConfigLexer.hpp"
#include "server/Socket.hpp"
#include "utils/utils.hpp"

ConfigParser::ConfigParser(): _codeError(NO_ERROR) {
	_directiveFunctions["error_page"] = &ConfigParser::_parseErrorPage;
	_directiveFunctions["root"] = &ConfigParser::_parseRoot;
	_directiveFunctions["index"] = &ConfigParser::_parseIndex;
	_directiveFunctions["allow_methods"] = &ConfigParser::_parseAllowMethods;
	_directiveFunctions["client_max_body_size"] = &ConfigParser::_parseClientMaxBodySize;
	_directiveFunctions["autoindex"] = &ConfigParser::_parseAutoIndex;
	_directiveFunctions["listen"] = &ConfigParser::_parseListen;
	_directiveFunctions["server_name"] = &ConfigParser::_parseServerName;
	_directiveFunctions["return"] = &ConfigParser::_parseReturn;
	_directiveFunctions["cgi_path"] = &ConfigParser::_parseCGIPath;
}

ConfigParser::ConfigParser(const ConfigParser &other)
{
	*this = other;
}

ConfigParser::~ConfigParser() { }

ConfigParser& ConfigParser::operator=(const ConfigParser& other)
{
	if (this == &other)
		return *this;
	_directiveFunctions = other._directiveFunctions;
	_parsedVirtualServers = other._parsedVirtualServers;
	_codeError = other._codeError;
	_error = other._error;
	return *this;
}

bool ConfigParser::parseConfigFile(const Context& mainContext) {
	return _parseMainContext(mainContext);
}

bool	ConfigParser::_parseMainContext(const Context& context) {
	std::vector<std::string> contexts;
	contexts.push_back(std::string("http"));
	std::vector<std::string> directives;
	if (!_parseAllowSubContexts(context, contexts))
		return false;
	if (context.getConstSubContexts().size() != 1) {
		_codeError = DUPLICATE_CONTEXT;
		_error = "\"http\" context is duplicate in main";
		return false;
	}
	if (!_parseDirectives(context, directives))
		return false;
	if (!_parseHttpContext(context.getConstSubContexts()[0]))
		return false;
	return true;
}

ConfigParser::codeError ConfigParser::getCodeError() const {
	return _codeError;
}

std::string ConfigParser::getError() const {
	return _error;
}

bool ConfigParser::_parseHttpContext(const Context& context) {
	std::vector<std::string> contexts;
	contexts.push_back(std::string("server"));
	std::vector<std::string> directives = splitWhiteSpace(\
		AUTHORIZED_DIRECTIVES_IN_HTTP_CONTEXT);

	if (!_parseAllowSubContexts(context, contexts))
		return false;
	if (context.getConstSubContexts().empty()) {
		_error = "no virtual server found";
		_codeError = NO_VIRTUAL_SERVER_FOUND;
		return false;
	}
	if (!_parseDirectives(context, directives))
		return false;
	if (!_parseServersContext(context.getConstSubContexts()))
		return false;
	return true;
}

bool ConfigParser::_parseServersContext(const std::vector<Context> &servers) {
	std::vector<std::string> contexts;
	contexts.push_back(std::string("location"));
	std::vector<std::string> directives = splitWhiteSpace(\
		AUTHORIZED_DIRECTIVES_IN_SERVER_CONTEXT);

	std::vector<Context>::const_iterator	it;
	for (it = servers.begin(); it != servers.end(); it++) {
		struct virtualServer server;
		server.ip = DEFAULT_VIRTUAL_SERVER_IP_ADDRESS;
		server.port = DEFAULT_VIRTUAL_SERVER_PORT;
		_parsedVirtualServers.push_back(server);
		if (!_parseAllowSubContexts(*it, contexts))
			return false;
		if (!_parseDirectives(*it, directives))
			return false;
		if (!_parseConflictingServerName())
			return false;
		if (!_parseLocationsContext(it->getConstSubContexts()))
			return false;
	}
	return true;
}

bool ConfigParser::_parseConflictingServerName() {
	const virtualServer& lastVirtualServer = \
		_parsedVirtualServers[_parsedVirtualServers.size() -1];

	std::vector<virtualServer>::const_iterator it;
	for (it = _parsedVirtualServers.begin(); it != _parsedVirtualServers.end() - 1; it++) {
		if (_isEqualVirtualServersIP(lastVirtualServer, *it)
			&& _hasSameServerName(lastVirtualServer.serverName, it->serverName)) {
			_codeError = CONFLICTING_SERVER_NAME;
			_error = "conflicting server name on "
					+ lastVirtualServer.ip + ":"
					+ toString(lastVirtualServer.port);
			return false;
		}
	}
	return true;
}

bool ConfigParser::_parseLocationsContext(const std::vector<Context> &locations) {
	std::vector<std::string> contexts;
	std::vector<std::string> directives = splitWhiteSpace(\
		AUTHORIZED_DIRECTIVES_IN_LOCATION_CONTEXT);

	std::vector<std::string> locationsURI;
	std::vector<Context>::const_iterator	it;
	for (it = locations.begin(); it != locations.end(); it++) {
		std::vector<std::string> arguments = splitWhiteSpace(it->getArguments());
		if (arguments.size() != 1) {
			_codeError = INVALID_ARGUMENTS;
			_error = "invalid arguments in \"" + it->getName() + "\" context";
			return false;
		}
		if (!_parseAllowSubContexts(*it, contexts) || !_parseDirectives(*it, directives))
			return false;
		if (std::find(locationsURI.begin(), locationsURI.end(),
					  arguments[0]) != locationsURI.end()) {
			_codeError = DUPLICATE_LOCATION;
			_error = "duplicate location \"" + arguments[0] + "\"";
			return false;
		}
		locationsURI.push_back(arguments[0]);
	}
	return true;
}

bool ConfigParser::_parseDirective(const std::string& name, const std::string& content) {
	bool	returnCode = true;

	directiveFunctionMap::const_iterator it = _directiveFunctions.find(name);

	if (it != _directiveFunctions.end())
		returnCode = (this->*(it->second))(content);
	if (!returnCode) {
		_codeError = INVALID_ARGUMENTS;
		_error = "invalid arguments in \"" + name + "\"";
	}
	return returnCode;
}

bool	ConfigParser::_parseAllowSubContexts(const Context& context,
											 const std::vector<std::string>& allowSubContexts)
{
	const std::vector<Context>& subContexts = context.getConstSubContexts();
	std::vector<Context>::const_iterator it;
	for (it = subContexts.begin(); it != subContexts.end(); it++) {
		if (std::find(allowSubContexts.begin(), allowSubContexts.end(),
					  it->getName()) == allowSubContexts.end()) {
			_codeError = FORBIDDEN_CONTEXT;
			_error = "\"" + it->getName() + "\" context is not allowed in "
					+ context.getName() + " context";
			return false;
		}
	}
	return true;
}

bool	ConfigParser::_parseDirectives(const Context& context,
									   const std::vector<std::string>& allow_directives) {
	std::map<std::string, std::string>::const_iterator it;
	const std::map<std::string, std::string>& directives = context.getDirectives();
	for (it = directives.begin(); it != directives.end(); it++) {
		if (std::find(allow_directives.begin(), allow_directives.end(),
					  it->first) == allow_directives.end()) {
			_codeError = FORBIDDEN_DIRECTIVE;
			_error = "\"" + it->first + "\" directive is not allowed in "
					+ context.getName() + " context";
			return false;
		}
		if (!_parseDirective(it->first, it->second))
			return false;
	}
	return true;
}

bool ConfigParser::_parseAllowMethods(const std::string &directiveContent) {
	std::vector<std::string> arguments = splitWhiteSpace(directiveContent);
	std::vector<std::string>::iterator it;

	if (arguments.empty())
		return false;
	for (it = arguments.begin(); it != arguments.end(); it++) {
		if (*it != "GET" && *it != "POST" && *it != "DELETE")
			return false;
	}
	return true;
}

bool ConfigParser::_parseAutoIndex(const std::string &directiveContent) {
	std::vector<std::string> arguments = splitWhiteSpace(directiveContent);

	if (arguments.size() != 1)
		return false;
	return (arguments[0] == "on" || arguments[0] == "off");
}

bool ConfigParser::_parseClientMaxBodySize(const std::string &directiveContent) {
	std::vector<std::string> arguments = splitWhiteSpace(directiveContent);
	char *endPtr;

	if (arguments.size() != 1)
		return false;
	std::strtol(arguments[0].c_str(), &endPtr, DECIMAL_BASE);
	if (errno == ERANGE || *endPtr != 'm' || *(endPtr + 1) != '\0')
		return false;
	return true;
}

bool	ConfigParser::_parseErrorPage(const std::string &directiveContent) {
	std::vector<std::string> arguments = splitWhiteSpace(directiveContent);

	if (arguments.size() < 2)
		return false;
	std::vector<std::string>::iterator	it;
	for (it = arguments.begin(); it != arguments.end() - 1; it++) {
		char *endPtr;
		long statusCode = std::strtol((*it).c_str(), &endPtr, DECIMAL_BASE);
		if (errno == ERANGE || *endPtr != '\0' ||
			statusCode < 300 || statusCode > 599)
			return false;
	}
	return true;
}

bool ConfigParser::_parseIndex(const std::string &directiveContent) {
	std::vector<std::string> arguments = splitWhiteSpace(directiveContent);

	return (!arguments.empty());
}

bool ConfigParser::_parseListen(const std::string &directiveContent) {
	std::vector<std::string> arguments = splitDelimiter(directiveContent, ':');
	char *endPtr;
	long port;

	if (std::count(directiveContent.begin(), directiveContent.end(), ':') != 1
		|| arguments.size() != 2)
		return false;
	port = std::strtol(arguments[1].c_str(), &endPtr, DECIMAL_BASE);
	if (errno == ERANGE || *endPtr != '\0'
		 || port < std::numeric_limits<u_int16_t>::min()
		 || port > std::numeric_limits<u_int16_t>::max())
		 return false;
	if (arguments[0] != "localhost" && !_parseIPAddress(arguments[0]))
		return false;
	_parsedVirtualServers[_parsedVirtualServers.size() - 1].ip = arguments[0];
	_parsedVirtualServers[_parsedVirtualServers.size() - 1].port = \
		static_cast<unsigned short>(port);
	return true;
}

bool ConfigParser::_parseIPAddress(const std::string &ipAddress) {
	std::vector<std::string>	bytesStrs = splitDelimiter(ipAddress, '.');
	char	*endPtr;
	long	byte;

	if (std::count(ipAddress.begin(), ipAddress.end(), '.') != 3
		|| bytesStrs.size() != 4)
		return false;
	std::vector<std::string>::iterator it;
	for (it = bytesStrs.begin(); it != bytesStrs.end(); it++) {
		byte = std::strtol((*it).c_str(), &endPtr, DECIMAL_BASE);
		if (errno == ERANGE || *endPtr != '\0' || byte < 0 || byte > 255)
			return false;
	}
	return true;
}

bool ConfigParser::_parseRoot(const std::string &directiveContent) {
	std::vector<std::string> arguments = splitWhiteSpace(directiveContent);

	return (arguments.size() == 1);
}

bool ConfigParser::_parseServerName(const std::string &directiveContent) {
	std::vector<std::string> arguments = splitWhiteSpace(directiveContent);

	if (arguments.empty())
		return false;
	_parsedVirtualServers[_parsedVirtualServers.size() - 1].serverName = arguments;
	return true;
}

bool ConfigParser::_parseReturn(const std::string &directiveContent) {
	std::vector<std::string> arguments = splitWhiteSpace(directiveContent);
	char	*endPtr;
	long	statusCode;

	if (arguments.size() != 2)
		return false;
	statusCode = std::strtol(arguments[0].c_str(), &endPtr, DECIMAL_BASE);
	if (errno == ERANGE || *endPtr != '\0'
		|| statusCode < 100 || statusCode > 599)
		return false;
	return true;
}

bool ConfigParser::_parseCGIPath(const std::string &directiveContent) {
	std::vector<std::string> arguments = splitWhiteSpace(directiveContent);

	if (arguments.empty())
		return false;
	std::vector<std::string>::const_iterator it;
	for (it = arguments.begin(); it != arguments.end(); it++) {
		std::vector<std::string> extensionPath = splitDelimiter(*it, ':');
		if (extensionPath.size() != 2)
			return false;
	}
	return true;
}

bool ConfigParser::_parseAlias(const std::string &directiveContent) {
	std::vector<std::string> arguments = splitWhiteSpace(directiveContent);

	return (arguments.size() == 1);
}

bool ConfigParser::_isEqualVirtualServersIP(const virtualServer& virtualServer1,
									   const virtualServer& virtualServer2) {
	return (virtualServer1.port == virtualServer2.port
	&& (virtualServer1.ip == virtualServer2.ip
		|| virtualServer1.ip == UNSPECIFIED_ADDRESS
		|| virtualServer2.ip == UNSPECIFIED_ADDRESS
		|| (virtualServer1.ip == LOCALHOST && virtualServer2.ip == LOOPBACK_IP)
		|| (virtualServer1.ip == LOOPBACK_IP && virtualServer2.ip == LOCALHOST)));
}

bool ConfigParser::_hasSameServerName(const std::vector<std::string>&	serverName1,
									  const std::vector<std::string>&	serverName2) {
	if (serverName1.empty() && serverName2.empty())
		return true;
	std::vector<std::string>::const_iterator it;
	for (it = serverName1.begin(); it != serverName1.end(); it++) {
		if (std::find(serverName2.begin(), serverName2.end(), *it)
			!= serverName2.end())
			return true;
	}
	return false;
}
