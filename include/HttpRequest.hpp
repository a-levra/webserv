#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include <string>
#include <map>

class HttpRequest {
	public:
		HttpRequest();
		HttpRequest(std::string raw);
		HttpRequest(const HttpRequest &other);
		virtual ~HttpRequest();
		HttpRequest &operator=(const HttpRequest &other);
		bool parse();
		void setRawRequest(std::string string);
		void displayRequest();

		void setVersion();
		void setStatusCode(int i);
		void setStatusMessage(std::string statusMessage);
		void setHeaders(std::string header, std::string content);
		void setBody(std::string body);
		void build();
		std::string getRawRequest();
		void setBodyFromFile(std::string path);
	private:
		std::string _method;
		std::string _path;
		std::string _version;
		std::map<std::string, std::string> _headers;
		std::string _body;
		std::string _rawRequest;
		bool parseMethod();
		bool parsePath();
		bool parseVersion();
		bool parseAllHeaders();
		bool parseHeader(const std::string &line);
		void display(std::string message);
		bool checkPathValidity(size_t spacePos);
		bool checkDoubleSpaces();
		int _statusCode;
		std::string _statusMessage;
};

#endif