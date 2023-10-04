#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <arpa/inet.h>
#include <poll.h>
#include <string>

class Socket
{
	public:
		Socket(void);
		Socket(const std::string& ip_address, int port);
		Socket(const Socket &other);
		~Socket(void);

		Socket &operator=(const Socket &other);

		void			binding(const std::string& ip_address, const int port);
		void			listening(void);
		void			closeFD(void);

		int				getFD(void) const;
		int				getPort(void) const;
		std::string		getIP(void) const;
		struct pollfd	getPollFd(const short events) const;

		void	setReUse(const int option);

	private:
		struct sockaddr_in	_address;
		std::string			_ip_adress;
		int					_port;
		int					_fd;

		uint32_t	_convertIPToBinary(std::string ip_address);
};

#endif
