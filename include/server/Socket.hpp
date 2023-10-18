#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <string>
# include <arpa/inet.h>
# include <poll.h>

# define UNSPECIFIED_ADDRESS "0.0.0.0"
# define LOCALHOST "localhost"
# define LOOPBACK_IP "127.0.0.1"

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
		unsigned short	getPort() const;
		std::string		getIPAddress() const;
		std::string 	getIPAndPort() const;
		struct pollfd	getPollFd(short events) const;

	private:
		bool				_calculateRawIPAddress();

		struct sockaddr_in	_address;
		std::string			_IPAddress;
		uint32_t			_rawIPAddress;
		unsigned short		_port;
		int					_fd;

};

#endif
