#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <sys/poll.h>

# include "server/Socket.hpp"
# include "server/Client.hpp"
# include "virtualServer/VirtualServer.hpp"

# define PERSISTENCE_TRIALS 5
# define PERSISTENCE_SLEEP_MS 500

# define CLIENT_TIMEOUT_MS 10000

# define READ_BUFFER_SIZE 1024

class Server {
    public:
		Server();
		Server(const Server &other);
		virtual ~Server();
		Server &operator=(const Server &other);

		bool	addVirtualServers(const std::vector<VirtualServer>& virtualServers);
		bool	listen();

		VirtualServer * getVirtualServer(const std::string& IPAddress,
										 const unsigned short port,
										 const std::string &serverName);
		void displayVirtualServers();

		static bool					exit;

	private:

		bool	_createVirtualServer(const VirtualServer& virtualServer);
		bool	_existListenerSocket(const std::string& IPAddress, unsigned short port);
		bool	_tryBindSocket(Socket &socket, const std::string& IPAddress,
							   unsigned short port);
		bool	_tryListenSocket(Socket &socket);
		bool	_acceptNewClient(struct pollfd listener);
		void	_handleSockets();
		bool	_handleClient(struct pollfd& pollSocket, size_t clientIndex);
		bool	_readClientRequest(Client& client);
		bool	_sendClientRequest(Client& client);
		void	_disconnectClient(std::vector<Client>::iterator clientIt);

		void	_printError(const std::string& error);

		std::vector<Socket>			_listeners;
		std::vector<struct pollfd>	_pollFd;
		std::vector<VirtualServer>	_virtualServers;
		std::vector<Client>			_clients;

};

#endif