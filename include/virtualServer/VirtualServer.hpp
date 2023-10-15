#ifndef VIRTUALSERVER_HPP
# define VIRTUALSERVER_HPP

#include <string>
#include <map>
#include <vector>

class Location;

class virtualServer {
	public:
		virtualServer();
		virtualServer(const virtualServer &other);
		virtual ~virtualServer();
		virtualServer &operator=(const virtualServer &other);

		void addLocation(const std::string &name, const Location &location);
		void removeLocation(const std::string &name);

		//getters and setters
		Location getLocation(const std::string &name) const;
		Location getLocationByUri(const std::string &uri) const;
		void setServerName(const std::vector<std::string>& serverName);
		void setIP(const std::string &ip);
		void setPort(int port);
		void setLocations(const std::map<std::string, Location> &locations);
		const std::vector<std::string>& getServerName() const;
		std::string getIP() const;
		short getPort() const;
		std::map<std::string, Location> getLocations() const;

	private:
		std::string _ipAddress;
		int			_port;
		std::vector<std::string> _serverName;
		std::map<std::string, Location> _locations;
};
#endif