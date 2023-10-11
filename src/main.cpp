#include "server/Server.hpp"

#include "config/ConfigParser.hpp"

#include "utils.hpp"
#include <iostream>
int main(int argc, char *argv[])
{
	if (argc == 3 && std::string(argv[1]) == "-t") {
		ConfigParser parser;
		parser.parseConfigFile(std::string(argv[2]));
		if (parser.getCodeError() != ConfigParser::NO_ERROR)
			std::cerr << "Error: " <<  parser.getError() << std::endl;
	}
	else {
		Server	webserv = Server();
		webserv.listen();
	}
	return 0;
}
