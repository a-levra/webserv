#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <sys/poll.h>

# include "Socket.hpp"
#include "virtualServer/VirtualServer.hpp"

class Server {
    public:
		Server();
		Server(const Server &other);
		virtual ~Server();
		Server &operator=(const Server &other);

		void	listen(void);
		void	addVirtualServer(const virtualServer &virtualServer);
	private:
		std::vector<Socket>	_listenerSockets;
		std::vector<struct pollfd>	_pollFd;
		std::vector<virtualServer>	_virtualServers;
};

#endif