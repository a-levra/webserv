#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <sys/poll.h>

# include "server/Socket.hpp"
# include "server/Client.hpp"
# include "virtualServer/VirtualServer.hpp"

# define PERSISTENCE_TRIALS 5
# define PERSISTENCE_SLEEP_MS 500

# define CLIENT_TIMEOUT_MS 5000

class Server {
    public:
		Server();
		Server(const Server &other);
		virtual ~Server();
		Server &operator=(const Server &other);

		bool	addVirtualServers(const std::vector<VirtualServer>& virtualServers);
		bool	listen();

		VirtualServer * getVirtualServer(const std::string &serverName);
		void displayVirtualServers();
		static std::string ft_inet_ntoa(uint32_t s_addr);
	private:

		bool	_createVirtualServer(const VirtualServer& virtualServer);
		bool	_existListenerSocket(const std::string& IPAddress, unsigned short port);
		bool	_tryBindSocket(Socket &socket, const std::string& IPAddress,
							   unsigned short port);
		bool	_tryListenSocket(Socket &socket);
		bool	_accept_new_client(struct pollfd listener);
		void	_check_revents_sockets(void);
		ssize_t	_read_persistent_connection(size_t client_index);

		Client* _getClientFromFD(int fd);
		void	_printError(const std::string& error);

		std::vector<Socket>	_listenerSockets;
		std::vector<struct pollfd>	_pollFd;
		std::vector<VirtualServer>	_virtualServers;
		std::vector<Client>			_clients;
};

#endif