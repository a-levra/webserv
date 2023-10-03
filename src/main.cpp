#include "server/Server.hpp"
#include "server/Socket.hpp"
#include <cstdlib>

int main(void)
{
	Server	webserv = Server();

	webserv.listen();
	return EXIT_SUCCESS;
}
