#ifndef VIRTUALSERVER_HPP
# define VIRTUALSERVER_HPP

#include <string>
#include <map>
#include <vector>
#include "Location.hpp"

class Location;

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
		void setServerName(const std::vector<std::string>& serverName);
		void setIP(const std::string &ip);
		void setPort(int port);
		void setLocations(const std::map<std::string, Location> &locations);
		std::vector<std::string> & getServerName();
		std::string getIP() const;
		short getPort() const;
		std::map<std::string, Location> getLocations() const;

		void display();
	private:
		std::string _ipAddress;
		int			_port;
		std::vector<std::string> _serverName;
		std::map<std::string, Location> _locations;
};
#endif