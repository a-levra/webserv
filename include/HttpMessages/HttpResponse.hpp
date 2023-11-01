#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include "AHttpMessage.hpp"
#include "HttpRequest.hpp"
#include "server/Server.hpp"

class HttpResponse : public AHttpMessage {
	public:
		HttpResponse(class HttpRequest& r);
		HttpResponse(const HttpResponse &other);
		virtual ~HttpResponse();

		HttpResponse &operator=(const HttpResponse &other);
		void build();

		std::string getResponse(Server &server, const Client& client);
		std::string & buildErrorPage(int errorCode);
		void setHeader(const std::string& header, const std::string& content);
		void generateBody();
		const std::string *getFirstValidIndex() const;
		void GenerateErrorBody();
		void buildGet();
		void buildPost();
		void getFileFromPostAndSaveIt();
		void ExtractImgInsideBoundaries(std::string *boundary,
										std::string &filename,
										std::string &fileContent) const;
	private:
		Location *_location;
		HttpRequest &_request;
		std::string getResource() const;
		const std::string *tryGetFile(const std::string &resource);
		void buildDelete();
		bool checkRequestMaxBodySize();
		std::string getSpecificErrorPage(int code);
};

#endif