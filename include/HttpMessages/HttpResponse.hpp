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
		void build(Location &location, std::string host, const HttpRequest &request);

		std::string getResponse(Server &server, HttpRequest &request);
		void buildErrorPage(int i);
		void setHeader(std::string header, std::string content);
		void generateBody(Location &location);
		const std::string * getFirstValidIndex(const Location &location) const;
		void GenerateErrorBody();
		void buildGet(Location &location);
		void buildPost(Location &location, const HttpRequest &request);
		void getFileFromPostAndSaveIt(HttpRequest request);
		void ExtractImgInsideBoudaries(const HttpRequest &request,
									   std::string *boundary,
									   std::string &filename,
									   std::string &fileContent) const;
};

#endif