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
		void buildRawMessage();
		bool getFileFromPostAndSaveIt();
		void ExtractImgInsideBoundaries(std::string *boundary,
										std::string &filename,
										std::string &fileContent) const;
		HttpRequest::REQUEST_VALIDITY getRequestValidity() const;
		const std::string& getCGIPath() const;
		const std::string& getCGIFile() const;
		const std::string& getCGIPathInfo() const;
		const std::pair<std::string, std::string>& getCGIExtension() const;
		HttpRequest& getRequest() const;

	private:
		Location *_location;
		HttpRequest &_request;
		std::string _cgiPath;
		std::string _cgiFile;
		std::string _cgiPathInfo;
		std::pair<std::string, std::string> _cgiExtension;
		std::string getResource() const;
		const std::string *tryGetFile(const std::string &resource);
		void buildDelete();
		bool checkRequestMaxBodySize();
		std::string getSpecificErrorPage(int code);

		void buildCGIGet();
		void buildCGIPost();
		bool findCGIExtension(const std::string& path);
		void extractCGIPath(const std::string& path,
							std::string::size_type extensionPos);
		bool searchCGIPath();
		void parseCGIOutput(const std::string& cgiOutput);

		bool buildListingDirectory();

};

#endif