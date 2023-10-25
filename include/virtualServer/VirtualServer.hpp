#ifndef VIRTUALSERVER_HPP
# define VIRTUALSERVER_HPP

#include <string>
#include <map>
#include <vector>

#include "Location.hpp"
#include "server/Socket.hpp"

# define DEFAULT_VIRTUAL_SERVER_IP_ADDRESS UNSPECIFIED_ADDRESS
# define DEFAULT_VIRTUAL_SERVER_PORT 80
# define DEFAULT_VIRTUAL_SERVER_NAME ""

class VirtualServer {
	public:
		VirtualServer();
		VirtualServer(const VirtualServer &other);
		virtual ~VirtualServer();
		VirtualServer &operator=(const VirtualServer &other);

		void addLocation(const std::string &name, const Location &location);
		void removeLocation(const std::string &name);

		//getters and setters
		Location * getLocation(const std::string &name);
		std::vector<std::string> & getServerName();
		std::string getIPAddress() const;
		unsigned short getPort() const;
		std::string getIPAndPort() const;
		std::map<std::string, Location> getLocations() const;

		void setServerName(const std::vector<std::string>& serverName);
		void setIPAddress(const std::string &ip);
		void setPort(int port);
		void setLocations(const std::map<std::string, Location> &locations);

		void display();
	private:
		std::string _ipAddress;
		unsigned short _port;
		std::vector<std::string> _serverName;
		std::map<std::string, Location> _locations;
};
#endif