#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <arpa/inet.h>

# include "server/Socket.hpp"
# include "HttpMessages/HttpRequest.hpp"



class Client {
public:
	Client();
	Client(int fd, struct sockaddr_in address, struct sockaddr_in entryAddress);
	Client(const Client &other);
	~Client();

	Client &operator=(const Client &other);

  	HttpRequest::REQUEST_VALIDITY checkRequestValidity();
	void	appendRawRequest(const std::string& rawRequest);
	void	setRawRequest(const std::string& rawRequest);
	HttpRequest	getRequest();
	int 		getFD();
	const std::string& getEntryIPAddress() const;
	unsigned short getEntryPort() const;
	time_t		getMSSinceLastActivity();
	time_t		updateLastActivity();
	int 		disconnect() const;

private:
	HttpRequest			_request;
	Socket				_socket;
  	std::string			_rawRequest;
  	std::string 		_entryIPAddress;
  	struct sockaddr_in	_entryAddress;
  	time_t				_lastActivity;
  	unsigned short		_entryPort;
};

#endif
