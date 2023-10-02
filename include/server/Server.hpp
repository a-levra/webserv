#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <sys/poll.h>

# include "Socket.hpp"

class Server {
    public:
		Server();
		Server(const Server &other);
		virtual ~Server();
		Server &operator=(const Server &other);

		void	listen(void);

	private:
		std::vector<Socket>	_listenerSockets;
		std::vector<struct pollfd>	_pollFd;
};

#endif