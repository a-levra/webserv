#include <cstdlib>
#include <unistd.h> //DEBUG
#include <cstdio> //DEBUG

#include "server/Server.hpp"
#include "server/Socket.hpp"
#include "virtualServer/Location.hpp"
#include "../include/utils/utils.hpp"


void generateSampleConfig(Server &server);

bool isLocalMode(int argc, char **argv);

int main(int argc, char **argv)
{
	bool localMode = isLocalMode(argc, argv);
	Server	webserv = Server(localMode);
	generateSampleConfig(webserv);
	webserv.displayVirtualServers();
	webserv.listen();
	return EXIT_SUCCESS;
}

bool isLocalMode(int argc, char **argv) {
	for (int i = 1; i < argc; i++) {
		if (std::string(argv[i]) == "--local" || std::string(argv[i]) == "-l")
			return true;
	}
	return false;
}

void generateSampleConfig(Server &server) {
	VirtualServer virtualServer;
	Location location;

	location.setRoot("/LeuleuWebsite");
	location.setIndex("index.html");
	location.setUri("contact.html");
	location.setClientMaxBodySize(1000000);
	location.setAutoindex(true);
//	location.addMethod("GET");
	virtualServer.setName("leuleu");
	virtualServer.setIP("localhost");
	virtualServer.setPort(8080);
	virtualServer.addLocation(location);
	coloredLog("leuleu Virtual server created", "", PURPLE);
	server.addVirtualServer(virtualServer);
//	server.displayVirtualServers();


}
