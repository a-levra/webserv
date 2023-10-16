#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <sys/poll.h>

# include "Socket.hpp"
# include "virtualServer/VirtualServer.hpp"

class Server {
    public:
		Server();
		Server(const std::vector<VirtualServer>& virtualServers);
		Server(const Server &other);
		virtual ~Server();
		Server &operator=(const Server &other);

		void	listen(void);
		void	addVirtualServer(const VirtualServer &virtualServer);
		Location * getLocation(const std::string &uri, VirtualServer &host);
		VirtualServer * getVirtualServer(const std::string &serverName);
		void displayVirtualServers();
	private:

		void	_accept_new_client(struct pollfd listener);
		void	_check_revents_sockets(void);
		ssize_t	_read_persistent_connection(size_t client_index);

		std::vector<Socket>	_listenerSockets;
		std::vector<struct pollfd>	_pollFd;
		std::vector<VirtualServer>	_virtualServers;
		template<class _Tp>
		const _Tp &isServerName(const std::string &basic_string);
		bool _localMode;
};

#endif