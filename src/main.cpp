#include "server/Server.hpp"

int main(void)
{
	Server	webserv = Server();

	webserv.listen();
	return (0);
}
