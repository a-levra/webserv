#include "server/Server.hpp"
#include "config/ConfigParser.hpp"

#include <iostream>

static bool	isParsingFlag(int argc, char *argv[]);
static void	parseConfigFile(const std::string& configFile);

int main(int argc, char *argv[])
{
	if (isParsingFlag(argc, argv))
		parseConfigFile(argv[2]);
	else {
		Server	webserv = Server();
		webserv.listen();
	}
	return EXIT_SUCCESS;
}

static bool	isParsingFlag(int argc, char *argv[]) {
	return (argc == 3 && std::string(argv[1]) == "-t");
}

static void	parseConfigFile(const std::string& configFile) {
	ConfigParser parser;
	parser.parseConfigFile(configFile);
	if (parser.getCodeError() != ConfigParser::NO_ERROR)
		std::cerr << "Syntax Error: " <<  parser.getError() << std::endl;
	else
		std::cout << "Syntax is ok" << std::endl;
}