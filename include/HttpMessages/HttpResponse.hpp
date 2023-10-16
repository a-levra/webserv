#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP
#include "AHttpMessage.hpp"
#include "HttpRequest.hpp"
#include "../server/Server.hpp"

class HttpResponse : public AHttpMessage {
	public:
		HttpResponse();
		HttpResponse(const HttpResponse &other);
		virtual ~HttpResponse();

		HttpResponse &operator=(const HttpResponse &other);
		void build();
		void build(Location &location, std::string host);

		std::string getResponse(Server &server, HttpRequest &request);
		void buildErrorPage(int i, Server &server);
		void setHeader(std::string header, std::string content);
		void generateBody(Location &location);
};

#endif