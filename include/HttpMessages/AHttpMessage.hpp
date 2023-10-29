#ifndef AHTTPMESSAGE_HPP
# define AHTTPMESSAGE_HPP

#include <string>
#include <map>

#define HTTP_VERSION "HTTP/1.1"
#define GENERIC_CSS_STYLE "<link rel=\"stylesheet\" type=\"text/css\" href=\"css/style.css\">"
#define NAVBAR "<div class=\"navbar\"><a href=\"/\">Home</a><a href=\"/upload\">Upload</a><a href=\"/delete\">Delete</a></div>"
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
		void setStatusMessage(std::string statusMessage);
		void setHeaders(std::string header, std::string content);
		std::string * getHeader(const std::string &header);
		void setBody(std::string body);
		const std::string & getBody() const;
		std::string getRawRequest();
		void setBodyFromFile(std::string path);
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