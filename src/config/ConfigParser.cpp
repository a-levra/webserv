#include "config/ConfigParser.hpp"
#include "config/ConfigLexer.hpp"
#include "utils.hpp"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <cerrno>

ConfigParser::ConfigParser(void): _codeError(NO_ERROR) {

}

ConfigParser::ConfigParser(const ConfigParser &other)
{
	*this = other;
}

ConfigParser::~ConfigParser(void)
{

}

ConfigParser &ConfigParser::operator=(const ConfigParser &other)
{
	if (this == &other)
		return (*this);
	return (*this);
}

int ConfigParser::parseConfigFile(const std::string &fileName) {
	ConfigLexer	lexer(fileName);
	if (lexer.getCodeError() != ConfigLexer::NO_ERROR) {
		std::cout << "Error: " << lexer.getError() << std::endl;
		return false;
	}
	if (_codeError != NO_ERROR)
		std::cout << _error << std::endl;
	if (!_parseMainContext(*lexer.getMainContext())) {
		std::cout << "Error: " << _error << std::endl;
		return false;
	}
//	if (!_parseHttpContext(&lexer.getMainContext()->getSubContexts()[0])) {
//		std::cout << "Error: " << _error << std::endl;
//		return false;
//	}
	return true;
}

/*
 * Dans main : context{http}  directives{};
 * Dans http : context {server}
 * 			   directives {error_page, root, index, allow_methods, client_max_body_size, autoindex};
 * Dans server: context {location}
 * 				directives {listen, server_name, error_page, root, index, allow_methods, client_max_body_size, autoindex, return};
 * 	Dans location: context {}
 * 				   directives {error_page, root, index ,allow_methods, client_max_body_size, autoindex, return};
 *
 */

bool	ConfigParser::_parseMainContext(const Context& context) {
	std::vector<std::string> contexts;
	contexts.emplace_back("http");
	std::vector<std::string> directives;
	if (!_checkSubContext(context, contexts))
		return false;
	if (context.getConstSubContexts().size() != 1) {
		_codeError = DUPLICATE_CONTEXT;
		_error = "\"http\" context is duplicate in main";
		return false;
	}
	if (!_checkDirectives(context, directives))
		return false;
	if (!_parseHttpContext(context.getConstSubContexts()[0]))
		return false;
	return true;
}

bool ConfigParser::_parseHttpContext(const Context& context) {
	std::vector<std::string> contexts;
	contexts.emplace_back("server");
	std::vector<std::string> directives = splitWhiteSpace("error_page "
														  "index root "
														  "allow_methods "
														  "client_max_body_size "
														  "autoindex ");
	if (!_checkSubContext(context, contexts))
		return false;
	if (!_checkDirectives(context, directives))
		return false;
	if (!_parseServersContext(context.getConstSubContexts()))
		return false;
	return true;
}

bool ConfigParser::_parseServersContext(const std::vector<Context> &servers) {
	std::vector<std::string> contexts;
	contexts.emplace_back("location");
	std::vector<std::string> directives = splitWhiteSpace("error_page "
														  "root index "
														  "allow_methods "
														  "client_max_body_size "
														  "autoindex listen "
														  "server_name return ");
	std::vector<Context>::const_iterator	it;
	for (it = servers.begin(); it != servers.end(); it++) {
		if (!_checkSubContext(*it, contexts))
			return false;
		if (!_checkDirectives(*it, directives))
			return false;
		if (!_parseLocationsContext(it->getConstSubContexts()))
			return false;
	}
	return true;
}

bool ConfigParser::_parseLocationsContext(const std::vector<Context> &locations) {
	std::vector<std::string> contexts;
	std::vector<std::string> directives = splitWhiteSpace("error_page "
														  "root index "
														  "allow_methods "
														  "client_max_body_size "
														  "autoindex "
														  "return ");
	std::vector<Context>::const_iterator	it;
	for (it = locations.begin(); it != locations.end(); it++) {
		if (splitWhiteSpace(it->getArguments()).size() != 1)
			return false;
		if (!_checkSubContext(*it, contexts))
			return false;
		if (!_checkDirectives(*it, directives))
			return false;
	}
	return true;

}

bool ConfigParser::_parseDirective(const std::string& name, const std::string& content) {
	bool	returnCode = true;

	if (name == "error_page")
		returnCode = _parseErrorPage(content);
	else if (name == "root")
		returnCode = _parseRoot(content);
	else if (name == "index")
		returnCode = _parseIndex(content);
	else if (name == "allow_methods")
		returnCode = _parseAllowMethods(content);
	else if (name == "client_max_body_size")
		returnCode = _parseClientMaxBodySize(content);
	else if (name == "autoindex")
		returnCode = _parseAutoIndex(content);
	else if (name == "listen")
		returnCode = _parseListen(content);
	else if (name == "server_name")
		returnCode = _parseServerName(content);
	else if (name == "return")
		returnCode = _parseReturn(content);
	if (!returnCode) {
		_codeError = INVALID_ARGUMENTS;
		_error = "invalid arguments in \"" + name + "\"";
	}
	return returnCode;
}

bool	ConfigParser::_checkSubContext(const Context& context,
									   const std::vector<std::string>& contexts)
{
	std::vector<Context>::const_iterator it;
	const std::vector<Context>& subContexts = context.getConstSubContexts();
	for (it = subContexts.begin(); it != subContexts.end(); it++) {
		if (std::find(contexts.begin(), contexts.end(), it->getName()) == contexts.end()) {
			_codeError = FORBIDDEN_CONTEXT;
			_error = "\"" + it->getName() + "\" context is not allowed in " + context.getName();
			return false;
		}
	}
	return true;
}

bool	ConfigParser::_checkDirectives(const Context& context, const std::vector<std::string>& allow_directives) {
	std::map<std::string, std::string>::const_iterator it;
	const std::map<std::string, std::string>& directives = context.getDirectives();
	for (it = directives.begin(); it != directives.end(); it++) {
		if (std::find(allow_directives.begin(), allow_directives.end(), it->first) == allow_directives.end()) {
			_codeError = FORBIDDEN_DIRECTIVE;
			_error = "\"" + it->first + "\" directive is not allowed in " + context.getName() + " context";
			return false;
		}
		if (!_parseDirective(it->first, it->second))
			return false;
	}
	return true;
}


// Directive

bool ConfigParser::_parseAllowMethods(const std::string &directiveContent) {
	std::vector<std::string> arguments = splitWhiteSpace(directiveContent);
	std::vector<std::string>::iterator it;

	if (arguments.empty())
		return false;
	for (it = arguments.begin(); it != arguments.end(); it++) {
		if (*it != "GET" || *it != "POST" || *it != "DELETE")
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
	std::strtol(arguments[0].c_str(), &endPtr, 10);
	if (errno == ERANGE || *(endPtr) != 'm' || *(endPtr + 1) != '\0')
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
		long statusCode = std::strtol((*it).c_str(), &endPtr, 10);
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
	port = std::strtol(arguments[1].c_str(), &endPtr, 10);
	if (errno == ERANGE || *endPtr != '\0'
		 || port < std::numeric_limits<uint16_t>::min()
		 || port > std::numeric_limits<uint16_t>::max())
		 return false;
	return (arguments[0] == "localhost" || _parseIPAddress(arguments[0]));
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
		byte = std::strtol((*it).c_str(), &endPtr, 10);
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

	return (!arguments.empty());
}

bool ConfigParser::_parseReturn(const std::string &directiveContent) {
	std::vector<std::string> arguments = splitWhiteSpace(directiveContent);
	char	*endPtr;
	long	statusCode;

	if (arguments.size() != 2)
		return false;
	statusCode = std::strtol(arguments[0].c_str(), &endPtr, 10);
	if (errno == ERANGE || *endPtr != '\0'
		|| statusCode < 100 || statusCode > 599)
		return false;
	return true;
}