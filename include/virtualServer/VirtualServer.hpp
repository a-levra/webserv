#ifndef VIRTUALSERVER_HPP
# define VIRTUALSERVER_HPP
#include <string>
#include <map>

class location;

class virtualServer {
	public:
		virtualServer();
		virtualServer(const virtualServer &other);
		virtual ~virtualServer();
		virtualServer &operator=(const virtualServer &other);

		void addLocation(const std::string &name, const location &location);
		void removeLocation(const std::string &name);

		//getters and setters
		location getLocation(const std::string &name) const;
		location getLocationByUri(const std::string &uri) const;
		void setName(const std::string &name);
		void setIP(const std::string &ip);
		void setPort(const short port);
		void setLocations(const std::map<std::string, location> &locations);
		std::string getName() const;
		std::string getIP() const;
		short getPort() const;
		std::map<std::string, location> getLocations() const;

	private:
		std::string _name;
		std::string _ip;
		short	_port;
		std::map<std::string, location> _locations;
};
#endif