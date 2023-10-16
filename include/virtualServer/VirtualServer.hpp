#ifndef VIRTUALSERVER_HPP
# define VIRTUALSERVER_HPP
#include <string>
#include <map>
#include "Location.hpp"

class VirtualServer {
	public:
		VirtualServer();
		VirtualServer(const VirtualServer &other);
		virtual ~VirtualServer();
		VirtualServer &operator=(const VirtualServer &other);

		void addLocation(Location &location);
		void removeLocation(const std::string &name);

		//getters and setters
		Location * getLocation(const std::string &name);
		void setName(const std::string &name);
		void setIP(const std::string &ip);
		void setPort(const short port);
		void setLocations(const std::map<std::string, Location> &locations);
		const std::string & getName() const;
		std::string getIP() const;
		short getPort() const;
		std::map<std::string, Location> getLocations() const;

		void display();
	private:
		std::string _name;
		std::string _ip;
		short	_port;
		std::map<std::string, Location> _locations;
};
#endif