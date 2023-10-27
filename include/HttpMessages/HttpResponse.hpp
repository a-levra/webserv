#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "AHttpMessage.hpp"
#include "HttpRequest.hpp"
#include "../server/Server.hpp"

class HttpResponse : public AHttpMessage {
	public:
		HttpResponse(class HttpRequest& r);
		HttpResponse(const HttpResponse &other);
		virtual ~HttpResponse();

		HttpResponse &operator=(const HttpResponse &other);
		void build(Location &location, const std::string& host);

		std::string getResponse(Server &server);
		std::string & buildErrorPage(int i);
		void generateBody(Location &location);
		const std::string * getFirstValidIndex(const Location &location) const;
		void GenerateErrorBody();
		void buildGet(Location &location);
		void buildPost(Location &location);
		void getFileFromPostAndSaveIt();
		void ExtractImgInsideBoundaries(std::string *boundary,
										std::string &filename,
										std::string &fileContent) const;
	private:
		HttpRequest &_request;
};

#endif