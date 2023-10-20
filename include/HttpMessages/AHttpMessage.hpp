#ifndef AHTTPMESSAGE_HPP
# define AHTTPMESSAGE_HPP

#include <string>
#include <map>

class AHttpMessage {
	public:
		AHttpMessage();
		AHttpMessage(std::string raw);
		AHttpMessage(const AHttpMessage &other);
		virtual ~AHttpMessage();
		AHttpMessage &operator=(const AHttpMessage &other);
		void setRawRequest(std::string string);
		void appendRawRequest(const std::string& rawRequest);
		void displayRequest();

		void setVersion();
		void setStatusCode(int i);
		void setStatusMessage(std::string statusMessage);
		void setHeaders(std::string header, std::string content);
		std::string getHeader(std::string header);
		void setBody(std::string body);
//		virtual void build() =0;
		std::string getRawRequest();
		void setBodyFromFile(std::string path);

	protected:
		std::string _method;
		std::string _path;
		std::string _version;
		std::map<std::string, std::string> _headers;
		std::string _body;
		std::string _rawMessage;
		void display(std::string message);
		int _statusCode;
		std::string _statusMessage;
};

#endif