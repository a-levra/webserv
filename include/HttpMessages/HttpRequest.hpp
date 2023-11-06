#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include "AHttpMessage.hpp"
#include "utils/utils.hpp"

#define CRLF "\r\n"
#define CRLF_SIZE 2
#define DOUBLE_CRLF_SIZE 4
//the shortest http request possible is : sizeof("GET / HTTP/1.1/r/n/r/n")
#define SHORTEST_HTTP_REQUEST_POSSIBLE 18
#define CONTENT_LENGTH "Content-Length"

class HttpRequest: public AHttpMessage{
	public:

		enum REQUEST_VALIDITY {
			NOT_PARSED_YET,
			VALID_AND_COMPLETE_REQUEST,
			INVALID_REQUEST,
			INCOMPLETE_REQUEST,
			VALID_AND_INCOMPLETE_REQUEST
		};



		enum LEXER_TOKENS{
			TOK_METHOD,
			TOK_REQUEST_URI,
			TOK_HTTP_VERSION,
			TOK_HEADERS,
			TOK_BODY,
		};

		enum ERRORS{

			/* lexer errors */
			RAW_MESSAGE_TOO_SHORT,
			NO_CLRF_FOUND,
			NO_CLRFCLRF_FOUND,
			REQUEST_LINE_INVALID,
			ALL_LEXER_TOKENS_VALID,

			/* parser errors */
			UNSUPPORTED_METHOD,
			REQUEST_URI_NOT_ALNUM,
			REQUEST_URI_FORBIDDEN_SYNTAX,
			INVALID_HTTP_VERSION,
			NO_COLON_FOUND_IN_HEADER,
			NO_VALUE_FOUND_FOR_HEADER,
			BODY_WITHOUT_CONTENT_LENGTH,
			BODY_LENGTH_NOT_MATCHING_CONTENT_LENGTH,

			/* other errors */
			PAYLOAD_TOO_LARGE_ERROR,
			UNKNOWN_ERROR
		};

		HttpRequest();
		HttpRequest(const std::string &raw);
		HttpRequest(const HttpRequest &other);
		virtual ~HttpRequest();

		HttpRequest &operator=(const HttpRequest &other);

		REQUEST_VALIDITY checkValidity();
		ERRORS autoLexer(std::map<enum LEXER_TOKENS, std::string> &lexerTokens);
		REQUEST_VALIDITY autoParser(std::map<enum LEXER_TOKENS, std::string> &lexerTokens);
		void parseRequestLine(std::string &method, std::string &requestUri, std::string &httpVersion);
		void parseMethod(const std::string &method);
		void parseHttpVersion(const std::string &httpVersion);
		void parseHttpHeaders(const std::string &headers);
		bool parseHeader(const std::string &line);
		void parseBody(const std::string &body);
		void parseRequestURI(const std::string &requestUri);
		void addError(ERRORS error);

		const std::string &getRequestUri();
		const REQUEST_VALIDITY & getValidity() const;
		bool isInvalid() const;
		bool canSendResponse() const;
		std::string getErrors();
		bool isChunked() const;
		void setChunked(bool isChunked);
	private:
		bool		_isChunked;
		REQUEST_VALIDITY _validity;
		std::vector<ERRORS> _errors;
		std::string getLexerParserError(HttpRequest::ERRORS validity);
		bool unchunkBody(std::string &unchunkedBody);
};

#endif