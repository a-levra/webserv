#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP
#include "AHttpMessage.hpp"
#include "utils/utils.hpp"

enum REQUEST_VALIDITY {
	VALID_and_COMPLETE,
	INVALID,
	NOT_COMPLETE
};

class HttpRequest: public AHttpMessage{
	public:

		HttpRequest();
		HttpRequest(const std::string &raw);
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
		enum REQUEST_VALIDITY checkValidity();
		const std::string &getPath() const;
		const REQUEST_VALIDITY & getValidity() const;
	private:
		enum REQUEST_VALIDITY _validity;
		std::string _pendingRawMessage;
};

#endif