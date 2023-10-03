#ifndef AHTTPPARSER_HPP
# define AHTTPPARSER_HPP

#include <string>
#include <map>

class AHttpParser {
	public:
		AHttpParser();
		AHttpParser(const AHttpParser &other);
		virtual ~AHttpParser();
		AHttpParser &operator=(const AHttpParser &other);
		void parse();
		void setRawRequest(const char *string);
		void displayRequest();
		class InvalidRequestException : public std::exception {
			const char * what() const throw() {
				return "Invalid Request";
			}
		};
		class InvalidPathException : public std::exception {
				const char * what() const throw() {
					return "Invalid path";
				}
		};
		class InvalidVersionException : public std::exception {
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
		void parseHeaders();
		void processLine(const std::string &line);
		void display(std::string message);
		void checkPathValidity(size_t spacePos);
};

#endif