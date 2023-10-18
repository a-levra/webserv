#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <string>
# include <arpa/inet.h>
# include <poll.h>

class Socket {
	public:
		Socket();
		Socket(const Socket &other);
		~Socket();

		Socket &operator=(const Socket &other);

		bool	initialize();
		bool	binding(const std::string& IPAddress, unsigned short port);
		bool	listening() const;
		void	closeFD();

		int				getFD() const;
		int				getPort() const;
		std::string		getIP() const;
		struct pollfd	getPollFd(short events) const;

	private:
		bool				_calculateRawIPAddress();

		struct sockaddr_in	_address;
		std::string			_IPAddress;
		uint32_t			_rawIPAddress;
		int					_port;
		int					_fd;

};

#endif
