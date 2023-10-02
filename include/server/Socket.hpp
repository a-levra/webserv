#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <arpa/inet.h>
#include <poll.h>

class Socket
{
	public:
		Socket(void);
		Socket(const Socket &other);
		~Socket(void);

		Socket &operator=(const Socket &other);

		void	binding(const sa_family_t family, const int port,
						const char* ip_address);
		void	listening(void);
		void	closeFd(void);

		int				getFd(void) const;
		struct pollfd	getPollFd(const short events) const;

		void	setReUse(const int option);


	private:
		int	_fd;
		struct sockaddr_in6	_address;

};

#endif
