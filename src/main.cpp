#include "server/Server.hpp"

#include "config/ConfigParser.hpp"

int main(void)
{
//	Server	webserv = Server();
//
//	webserv.listen();
	ConfigParser test;

	test.parseConfigFile("/home/tdameros/dev/42-webserv/webserv.conf");
	return 0;
}
