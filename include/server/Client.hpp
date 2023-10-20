#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "HttpMessages/HttpRequest.hpp"

# include <arpa/inet.h>

// TODO : remove
class Server;

class Client
{
public:
	Client();
	Client(int fd, struct sockaddr_in address, struct sockaddr_in entryAddress);
	Client(const Client &other);
	~Client();

	Client &operator=(const Client &other);

	void	appendRawRequest(const std::string& rawRequest);
	REQUEST_VALIDITY checkRequestValidity();
	HttpRequest	getRequest();
	int 		getFD();
	time_t		getMSSinceLastActivity();

private:
	static std::string _rawIPAddressToStrIPAddress(uint32_t rawIPAddress);
	static unsigned short	_rawPortToPort(uint16_t rawPort);

	HttpRequest			_request;
	struct sockaddr_in	_address;
	struct sockaddr_in	_entryAddress;
	std::string 		_IPAddress;
	std::string 		_entryIPAddress;
	time_t				_lastActivity;
	int					_fd;
	unsigned short		_port;
	unsigned short		_entryPort;
};

#endif
