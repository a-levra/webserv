#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include <string>
#include <map>

class HttpRequest {
	public:
		HttpRequest();
		HttpRequest(const HttpRequest &other);
		virtual ~HttpRequest();
		HttpRequest &operator=(const HttpRequest &other);
		void parse();
		void setRawRequest(const char *string);
		void displayRequest();
		class InvalidRequestException : public std::exception {
			const char * what() const throw() {
				return "Invalid Request";
			}
		};
		class InvalidPathException : public InvalidRequestException {
				const char * what() const throw() {
					return "Invalid path";
				}
		};
		class InvalidMethodException : public InvalidRequestException {
				const char * what() const throw() {
					return "Invalid method (or not supported)";
				}
		};
		class InvalidVersionException : public InvalidRequestException {
				const char * what() const throw() {
					return "Invalid version";
				}
		};

	private:
		std::string _method;
		std::string _path;
		std::string _version;
		std::map<std::string, std::string> _headers;
		std::string _body;
		std::string _rawRequest;
		bool _isValid;
		void parseMethod();
		void parsePath();
		void parseVersion();
		void parseAllHeaders();
		void parseHeader(const std::string &line);
		void display(std::string message);
		void checkPathValidity(size_t spacePos);
		void checkDoubleSpaces();
};

#endif