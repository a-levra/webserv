#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <netinet6/in6.h>
# include <sys/poll.h>

class Server {
    public:
		Server();
		Server(const Server &other);
		virtual ~Server();
		Server &operator=(const Server &other);

	private:
		int	listeningSocket;
		struct sockaddr_in6	sockAddress;
		std::vector<struct pollfd>	clients;
};

#endif