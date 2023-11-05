#ifndef AHTTPMESSAGE_HPP
# define AHTTPMESSAGE_HPP

#include <string>
#include <map>

#define HTTP_VERSION "HTTP/1.1"
#define GENERIC_CSS_STYLE "<link rel=\"stylesheet\" type=\"text/css\" href=\"css/style.css\">"
#define NAVBAR "<div class=\"navbar\"><a href=\"/\">Home</a><a href=\"/upload\">Upload</a><a href=\"/delete\">Delete</a></div>"

#define OK 200
#define BAD_REQUEST 400
#define NOT_FOUND 404
#define METHOD_NOT_ALLOWED 405
#define PAYLOAD_TOO_LARGE 413
#define INTERNAL_SERVER_ERROR 500
#define NOT_IMPLEMENTED 501
#define HTTP_VERSION_NOT_SUPPORTED 505

class AHttpMessage {
	public:
		AHttpMessage();
		AHttpMessage(std::string raw);
		AHttpMessage(const AHttpMessage &other);
		virtual ~AHttpMessage();
		AHttpMessage &operator=(const AHttpMessage &other);
		void setRawRequest(std::string string);
		void displayRequest();

		void setVersion();
		void setStatusCode(int i);
		int getStatusCode();
		void setStatusMessage(std::string statusMessage);
		void setHeaders(std::string header, std::string content);
		const std::map<std::string, std::string>& getHeaders() const;
		std::string * getHeader(const std::string &header);
		void setBody(std::string body);
		const std::string & getBody() const;
		std::string getRawRequest();
		void appendBody(const std::string &message);
		const std::string & getMethod() const;

	protected:
		std::string _method;
		std::string _requestUri;
		std::string _httpVersion;
		std::map<std::string, std::string> _headers;
		std::string _body;
		std::string _rawMessage;
		void display(std::string message);
		int _statusCode;
		std::string _statusMessage;
};

#endif