#include "HttpMessages/HttpRequest.hpp"
#include "logger/logging.hpp"

#include <stdlib.h>
#include <vector>
#include <map>

#define MAX_METHOD_LENGTH 6 // "DELETE" is the longest (supported) method

HttpRequest::HttpRequest(void) : AHttpMessage() {
	_validity = NOT_PARSED_YET;
	_isChunked = false;
}

HttpRequest::HttpRequest(const std::string &raw) : AHttpMessage(raw) {
	_validity = NOT_PARSED_YET;
	_isChunked = false;
}

HttpRequest::HttpRequest(const HttpRequest &other) : AHttpMessage(other) { *this = other; }

HttpRequest::~HttpRequest(void) {}

HttpRequest &HttpRequest::operator=(const HttpRequest &other) {
	if (this != &other) {
		_method = other._method;
		_requestUri = other._requestUri;
		_httpVersion = other._httpVersion;
		_headers = other._headers;
		_body = other._body;
		_rawMessage = other._rawMessage;
		_statusCode = other._statusCode;
		_statusMessage = other._statusMessage;
		_validity = other._validity;
		_errors = other._errors;
		_isChunked = other._isChunked;
	}
	return (*this);
}

HttpRequest::REQUEST_VALIDITY HttpRequest::checkValidity() {
	_validity = NOT_PARSED_YET; //mandatory when reusing the same object

	logging::debug(B_BLUE "Checking request" COLOR_RESET);
	_errors.clear();
	std::map<enum LEXER_TOKENS, std::string> lexerToken;
	ERRORS lexerValidity = autoLexer(lexerToken);
	getLexerParserError(lexerValidity);
	if (lexerValidity != ALL_LEXER_TOKENS_VALID) {
		_errors.push_back(lexerValidity);
		logging::debug(B_RED "Lexer error : " THIN COLOR_RESET + getErrors());
		_validity = INCOMPLETE_REQUEST;
		return _validity;
	}
	autoParser(lexerToken);
	if (!_errors.empty())
		logging::debug(B_RED "Pars errors : " THIN COLOR_RESET + getErrors());
	return (_validity);
}

enum HttpRequest::ERRORS HttpRequest::autoLexer
	(std::map<enum LEXER_TOKENS, std::string> &lexerTokens) {

	if (_rawMessage.size() <= SHORTEST_HTTP_REQUEST_POSSIBLE)
		return (RAW_MESSAGE_TOO_SHORT);

	std::string::size_type firstClrfPosition;
	std::string::size_type doubleClrfPos;

	firstClrfPosition = _rawMessage.find(CRLF);
	doubleClrfPos = _rawMessage.find(CRLF CRLF);

	if (firstClrfPosition == std::string::npos || doubleClrfPos == std::string::npos) {
		return ((firstClrfPosition == std::string::npos ? NO_CLRF_FOUND : NO_CLRFCLRF_FOUND));
	}

	std::vector<std::string> requestLine = splitWhiteSpace(_rawMessage.substr(0, firstClrfPosition));
	if (requestLine.size() == 3) {
		lexerTokens[TOK_METHOD] = requestLine[0];
		lexerTokens[TOK_REQUEST_URI] = requestLine[1];
		lexerTokens[TOK_HTTP_VERSION] = requestLine[2];
		lexerTokens[TOK_HEADERS] =
			_rawMessage.substr(firstClrfPosition + CRLF_SIZE, doubleClrfPos - firstClrfPosition - CRLF_SIZE);
		lexerTokens[TOK_BODY] = _rawMessage.substr(doubleClrfPos + DOUBLE_CRLF_SIZE);
		return (ALL_LEXER_TOKENS_VALID);
	}
	return (REQUEST_LINE_INVALID);
}

HttpRequest::REQUEST_VALIDITY HttpRequest::autoParser(std::map<enum LEXER_TOKENS, std::string> &lexerTokens) {
	parseRequestLine(lexerTokens[TOK_METHOD], lexerTokens[TOK_REQUEST_URI], lexerTokens[TOK_HTTP_VERSION]);
	parseHttpHeaders(lexerTokens[TOK_HEADERS]);
	parseBody(lexerTokens[TOK_BODY]);
	if (_validity == NOT_PARSED_YET)
		_validity = VALID_AND_COMPLETE_REQUEST;
	return _validity;
}

void HttpRequest::parseRequestLine(std::string &method, std::string &requestUri, std::string &httpVersion) {
	parseMethod(method);
	parseRequestURI(requestUri);
	parseHttpVersion(httpVersion);
}

void HttpRequest::parseMethod(const std::string &method) {
	if (method != "GET" &&
		method != "POST" &&
		method != "DELETE") {
		_validity = INVALID_REQUEST;
		_errors.push_back(UNSUPPORTED_METHOD);
	}
	_method = method;
}

void HttpRequest::parseRequestURI(const std::string &requestUri) {
	//requestUri must be alphanumeric and can contain only '/', '.' and '-'
	//requestUri cannot contain ".." or "//"

	//it's useful to save the original requestUri first for error logging purposes
	_requestUri = requestUri;
	size_t requestUriSize = requestUri.size();
	const char *c_str_uri = requestUri.c_str();
	for (size_t i = 0; i < requestUriSize; i++) {
		if (!isalnum(c_str_uri[i]) && c_str_uri[i] != '/' && c_str_uri[i] != '.' && c_str_uri[i] != '-') {
			_validity = INVALID_REQUEST;
			_errors.push_back(REQUEST_URI_NOT_ALNUM);
			return;
		}

		if ((i + 1 < requestUriSize) && (
			(c_str_uri[i] == '.' && c_str_uri[i + 1] == '.') ||
				((c_str_uri[i] == '/') && (c_str_uri[i + 1] == '/'))
		)) {
			_validity = INVALID_REQUEST;
			_errors.push_back(REQUEST_URI_FORBIDDEN_SYNTAX);
			return;
		}

	}
}

void HttpRequest::parseHttpVersion(const std::string &httpVersion) {
	if (httpVersion != HTTP_VERSION) {
		_validity = INVALID_REQUEST;
		_errors.push_back(INVALID_HTTP_VERSION);
	}
	_httpVersion = httpVersion;
}

void HttpRequest::parseHttpHeaders(const std::string &headers) {
	if (headers.empty())
		return;

	std::vector<std::string> headersVector = splitDelimiter(headers, CRLF);

	std::vector<std::string>::iterator it;
	for (it = headersVector.begin(); it != headersVector.end(); it++) {
		if (!parseHeader(*it)) {
			return;
		}
	}

}

bool HttpRequest::parseHeader(const std::string &line) {
	size_t semicolPos = line.find(':');
	if (semicolPos == std::string::npos) {
		_validity = INVALID_REQUEST;
		_errors.push_back(NO_COLON_FOUND_IN_HEADER);
		return false;
	}
	std::string headerValue = line.substr(semicolPos + 1);
	headerValue = trim(headerValue);
	if (headerValue.empty()) {
		_validity = INVALID_REQUEST;
		_errors.push_back(NO_VALUE_FOUND_FOR_HEADER);
		return false;
	}
	std::string headerName = line.substr(0, semicolPos);
	headerName = trim(headerName);
	_headers[headerName] = headerValue;
	return true;
}

void HttpRequest::parseBody(const std::string &body) {
	_body = body;
	if (this->getHeader("Transfer-Encoding") && this->getHeader("Transfer-Encoding")->find("chunked") != std::string::npos) {
		this->setChunked(true);
		logging::debug(B_BLUE "Chunked transfer encoding detected" THIN);
	}
	if (body.empty() && std::strtod(_headers[CONTENT_LENGTH].c_str(), 0) == 0) {
		_body = body;
		return;
	}
	if (!this->isChunked()){

	if (body.empty()) {
		_errors.push_back(BODY_WITHOUT_CONTENT_LENGTH);
		_validity = INVALID_REQUEST;
	}
	if (std::strtod(_headers[CONTENT_LENGTH].c_str(), 0) > _body.size()) {
		_errors.push_back(BODY_LENGTH_NOT_MATCHING_CONTENT_LENGTH);
		_validity = VALID_AND_INCOMPLETE_REQUEST;
	}
	if (std::strtod(_headers[CONTENT_LENGTH].c_str(), 0) < _body.size()) {
		_errors.push_back(BODY_LENGTH_NOT_MATCHING_CONTENT_LENGTH);
		_validity = INVALID_REQUEST;
	}
	}
	std::string unchunkedBody;
	if (this->isChunked()) {
		unchunkBody(unchunkedBody);
		if (_body.empty()) {
			_validity = INVALID_REQUEST;
		}
	}
}

bool HttpRequest::isInvalid() const {
	return (_validity == INVALID_REQUEST);
}

std::string HttpRequest::getErrors() {
	std::string res;
	std::vector<ERRORS>::iterator it;
	it = _errors.begin();
	for (; it != _errors.end(); it++) {
		res.append(getLexerParserError(*it) + "</h3><h3>");
	}
	return res;
}

std::string HttpRequest::getLexerParserError(HttpRequest::ERRORS validity) {
	switch (validity) {
		case RAW_MESSAGE_TOO_SHORT: return ("Raw message too short : " + _rawMessage);
			break;
		case NO_CLRF_FOUND: return ("No CRLF found in raw message : " + _rawMessage);
			break;
		case NO_CLRFCLRF_FOUND: return ("No CLRFCLRF found in raw message : " + _rawMessage);
			break;
		case REQUEST_LINE_INVALID: return ("Request line invalid : " + _rawMessage);
			break;
		case ALL_LEXER_TOKENS_VALID: return ("Lexer tokens are valid");
			break;
		case UNSUPPORTED_METHOD: return ("Unsupported method : " "\"" + _method + "\"");
			break;
		case REQUEST_URI_NOT_ALNUM: return ("Request URI not alphanumeric : " "\"" + _requestUri + "\"");
			break;
		case REQUEST_URI_FORBIDDEN_SYNTAX:
			return ("Request URI : \"..\" or \"//\" forbidden : " "\"" + _requestUri + "\"");
			break;
		case INVALID_HTTP_VERSION: return ("Invalid HTTP version : " "\"" + _httpVersion + "\"");
			break;
		case NO_COLON_FOUND_IN_HEADER: return ("No colon found in header : " "\"" + _rawMessage + "\"");
			break;
		case NO_VALUE_FOUND_FOR_HEADER: return ("No value found for header : " "\"" + _rawMessage + "\"");
			break;
		case BODY_WITHOUT_CONTENT_LENGTH: return ("Body without content length : " "\"" + _rawMessage + "\"");
			break;
		case BODY_LENGTH_NOT_MATCHING_CONTENT_LENGTH:
			return ("Body length not matching Content-Length :"
					"<h4>Body size : " +
				toString(_body.size()) +
				"</h4><h4>Content-Length : " +
				_headers[CONTENT_LENGTH] + "</h4>");
			break;
		case PAYLOAD_TOO_LARGE_ERROR: return ("Payload too large");
			break;
		default: return ("Unknown error");
			break;
	}
}

const std::string &HttpRequest::getRequestUri() {
	return _requestUri;
}

const HttpRequest::REQUEST_VALIDITY &HttpRequest::getValidity() const {
	return _validity;
}

bool HttpRequest::canSendResponse() const
{
	return (_validity == HttpRequest::INVALID_REQUEST
			|| _validity == HttpRequest::VALID_AND_COMPLETE_REQUEST
			|| _validity == HttpRequest::VALID_AND_INCOMPLETE_REQUEST);
}

void HttpRequest::addError(HttpRequest::ERRORS error) {
	_errors.push_back(error);
}

bool HttpRequest::isChunked() const {
	return _isChunked;
}

void HttpRequest::setChunked(bool isChunked) {
	_isChunked = isChunked;
}

bool HttpRequest::unchunkBody(std::string &unchunkedBody) {
	bool complete = false;
	std::vector<std::string> tok = splitDelimiter(_body, CRLF);
	std::vector<std::string>::const_iterator it;
	std::string strChunkSize;
	size_t chunkSize;
	for (it = tok.begin(); it != tok.end(); it++){
		if (strChunkSize.empty()){
			strChunkSize = *it;
			std::istringstream iss(strChunkSize);
			iss >> std::hex >> chunkSize;
			logging::debug("Chunk size : Ox" + *it + "(" + toString(chunkSize) + ")");
			if (chunkSize == 0 && it + 1 == tok.end()){
				complete = true;
				break;
			}
		}
		else{
			logging::debug("Chunk : " + *it);
			unchunkedBody.append(*it);
			size_t delta = chunkSize - it->size();
			while (delta/2 > 0){
				unchunkedBody.append(CRLF);
				delta -= 2;
			}
			strChunkSize = "";
		}
	}
	_body = unchunkedBody;
	if (complete)
		_validity = VALID_AND_COMPLETE_REQUEST;
	else
		_validity = VALID_AND_INCOMPLETE_REQUEST;
	return true;
}
