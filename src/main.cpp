#include <cstdlib>
#include <unistd.h> //DEBUG
#include <cstdio> //DEBUG

#include <string>
#include "server/Server.hpp"
#include "config/ConfigLexer.hpp"
#include "config/ConfigFactory.hpp"
#include "config/ConfigParser.hpp"
#include <iostream>

static bool	isParsingFlag(int argc, char *argv[]);
static bool parseConfigFile(const ConfigLexer& lexer);
static int 	testConfigFile(const std::string& configFile);
static int	runServer(const std::string& configFile);

int main(int argc, char **argv)
{
	if (!isParsingFlag(argc, argv) && argc != 2) {
		std::cerr << "Usage: ./webserv *.conf" << std::endl;
		return EXIT_FAILURE;
	}
	if (isParsingFlag(argc, argv))
		return testConfigFile(argv[2]);
	else
		return runServer(argv[1]);
	return EXIT_FAILURE;
}


static bool	isParsingFlag(int argc, char *argv[]) {
	return (argc == 3 && std::string(argv[1]) == "-t");
}

static int	testConfigFile(const std::string& configFile) {
	ConfigLexer	lexer(configFile);
	if (!parseConfigFile(lexer))
		return EXIT_FAILURE;
	std::cout << "Syntax is ok" << std::endl;
	return EXIT_SUCCESS;
}

static int runServer(const std::string& configFile) {
	Server			webserv;

	{
		ConfigLexer	lexer = ConfigLexer(configFile);
		if (!parseConfigFile(lexer))
			return EXIT_FAILURE;
		lexer.getMainContext().inheritDirectives();
		webserv = Server();
		if (!webserv.addVirtualServers(ConfigFactory::createVirtualServers(
				lexer.getMainContext()))) {
			std::cerr << "webserv: failed to launch virtual servers" << std::endl;
			return EXIT_FAILURE;
		}
	}
	webserv.listen();
	return EXIT_FAILURE;
}

static bool parseConfigFile(const ConfigLexer& lexer) {
	ConfigParser parser;

	if (lexer.getCodeError() != ConfigLexer::NO_ERROR) {
		std::cerr << "Lexical Error: " <<  lexer.getError() << std::endl;
		return false;
	}
	parser.parseConfigFile(lexer.getConstMainContext());
	if (parser.getCodeError() != ConfigParser::NO_ERROR) {
		std::cerr << "Syntax Error: " <<  parser.getError() << std::endl;
		return false;
	}
	return true;
}