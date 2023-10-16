#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP
#include "AHttpMessage.hpp"
#include "../utils/utils.hpp"

class HttpRequest: public AHttpMessage{
	public:
		HttpRequest();
		HttpRequest(std::string raw);
		HttpRequest(const HttpRequest &other);
		virtual ~HttpRequest();

		HttpRequest &operator=(const HttpRequest &other);
		bool parse();
		bool parseMethod();
		bool parsePath();
		bool parseVersion();
		bool parseAllHeaders();
		bool parseHeader(const std::string &line);
		bool checkPathValidity(size_t spacePos);
		bool checkDoubleSpaces();
		void build(void);

		const std::string &getPath();
};

#endif