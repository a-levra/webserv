#include "server/Server.hpp"

#include "server/parser/ConfigParser.hpp"

int main(void)
{
//	Server	webserv = Server();
//
//	webserv.listen();
	ConfigParser test;

	test.parseConfigFile("webserv.conf");
	return (0);
}
