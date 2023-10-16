#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <string>
# include <arpa/inet.h>
# include <poll.h>

class Socket
{
	public:
		Socket(void);
		Socket(const Socket &other);
		~Socket(void);

		Socket &operator=(const Socket &other);

		void	binding(std::string ip, int port);
		void	listening(void);
		void	closeFD(void);

		int				getFD(void) const;
		int 			getPort(void) const;
		std::string 	getIP(void) const;
		struct pollfd	getPollFd(const short events) const;

		void	setReUse(const int option);
		uint32_t	_convertIPToBinary(std::string ip_address);


	private:
		int	_fd;
		struct sockaddr_in _address;
		std::string	_ip;
		int			_port;



};

#endif
