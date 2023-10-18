#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <sys/poll.h>

# include "Socket.hpp"
# include "virtualServer/VirtualServer.hpp"

class Server {
    public:
		Server();
		Server(const Server &other);
		virtual ~Server();
		Server &operator=(const Server &other);

		bool	loadVirtualServers(const std::vector<VirtualServer>& virtualServers);
		bool	listen();

		VirtualServer * getVirtualServer(const std::string &serverName);
		void displayVirtualServers();
	private:

		bool	_accept_new_client(struct pollfd listener);
		void	_check_revents_sockets(void);
		ssize_t	_read_persistent_connection(size_t client_index);
		bool	_createVirtualServer(const VirtualServer& virtualServer);
		void	_printVirtualServerError(const std::string& function,
										 const VirtualServer &virtualServer);

		bool	_tryInitializeSocket(Socket &socket, const VirtualServer& virtualServer);
		bool	_tryBindSocket(Socket &socket, const VirtualServer& virtualServer);
		bool	_tryListenSocket(Socket &socket, const VirtualServer& virtualServer);

		std::vector<Socket>	_listenerSockets;
		std::vector<struct pollfd>	_pollFd;
		std::vector<VirtualServer>	_virtualServers;
};

#endif