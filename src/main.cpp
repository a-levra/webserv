#include "server/Server.hpp"

#include "config/ConfigParser.hpp"

#include "utils.hpp"
#include <iostream>
int main(int argc, char *argv[])
{
	if (argc == 3 && std::string(argv[1]) == "-t") {
		ConfigParser test;
		test.parseConfigFile(std::string(argv[2]));
	}
	else {
		ConfigParser test;
		test.parseConfigFile("/Users/tdameros/dev/42-webserv/webserv.conf");
		Server	webserv = Server();
		webserv.listen();
	}
	return 0;
}
