#include "HttpMessages/HttpRequest.hpp"

#include <iostream>
#include <stdlib.h>
#include <map>

#define MAX_METHOD_LENGTH 6 // "DELETE" is the longest (supported) method

HttpRequest::HttpRequest(void) : AHttpMessage() {
	_validity = NOT_PARSED_YET;
}

HttpRequest::HttpRequest(const std::string &raw) : AHttpMessage(raw) {
	_validity = NOT_PARSED_YET;
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
	}
	return (*this);
}

HttpRequest::REQUEST_VALIDITY HttpRequest::checkValidity() {
	_validity = NOT_PARSED_YET; //mandatory when reusing the same object

	std::map<enum LEXER_TOKENS, std::string> lexerToken;
	ERRORS lexerValidity = autoLexer(lexerToken);
	logLexerParserError(lexerValidity);
	if (lexerValidity != ALL_LEXER_TOKENS_VALID){
		_errors.push_back(lexerValidity);
		_validity = INCOMPLETE_REQUEST;
		return _validity;
	}

	autoParser(lexerToken);
	logErrors();
	return (_validity);
}

enum HttpRequest::ERRORS HttpRequest::autoLexer
	(std::map<enum LEXER_TOKENS,std::string> &lexerTokens){

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
		lexerTokens[TOK_HEADERS] = _rawMessage.substr(firstClrfPosition + CRLF_SIZE, doubleClrfPos - firstClrfPosition - CRLF_SIZE);
		lexerTokens[TOK_BODY] = _rawMessage.substr(doubleClrfPos + DOUBLE_CRLF_SIZE);
		coloredLog("\"" + _rawMessage.substr(doubleClrfPos + DOUBLE_CRLF_SIZE) + "\"", "", YELLOW);
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
		method != "DELETE"){
		_validity = INVALID_REQUEST;
		_errors.push_back(UNSUPPORTED_METHOD);
	}
	_method = method;
}

void HttpRequest::parseRequestURI(const std::string &requestUri) {
	//requestUri must be alphanumeric and can contain only '/', '.' and '-'
	//requestUri cannot contain ".." or "//"

	size_t requestUriSize = requestUri.size();
	const char * c_str_uri = requestUri.c_str();
	for (size_t i = 0; i < requestUriSize; i++) {
		if (!isalnum(c_str_uri[i]) && c_str_uri[i] != '/' && c_str_uri[i] != '.' && c_str_uri[i] != '-'){
			_validity = INVALID_REQUEST;
			_errors.push_back(REQUEST_URI_NOT_ALNUM);
			return;
		}

		if (( i + 1 < requestUriSize) && (
			(c_str_uri[i] == '.' && c_str_uri[i + 1] == '.') ||
				((c_str_uri[i] == '/') && (c_str_uri[i + 1] == '/'))
		)){
			_validity = INVALID_REQUEST;
			_errors.push_back(REQUEST_URI_FORBIDDEN_SYNTAX);
			return;
		}

	}
	_requestUri = requestUri;
}

void HttpRequest::parseHttpVersion(const std::string &httpVersion) {
	if (httpVersion != HTTP_VERSION){
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
	for (it = headersVector.begin(); it != headersVector.end(); it++){
		if (!parseHeader(*it)){
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
	if (semicolPos + 2 >= line.size()) {
		_validity = INVALID_REQUEST;
		_errors.push_back(NO_VALUE_FOUND_FOR_HEADER);
		return false;
	}
	std::string headerName = line.substr(0, semicolPos);
	std::string headerValue = line.substr(semicolPos + 2);
	_headers[trim(headerName)] = trim(headerValue);
	return true;
}

void HttpRequest::parseBody(const std::string &body){
	if (body.empty() && std::strtod(_headers[CONTENT_LENGTH].c_str(), 0) == 0){
		_body = body;
		return;
	}
	if (body.empty()){
		_errors.push_back(BODY_WITHOUT_CONTENT_LENGTH);
		_validity = INVALID_REQUEST;
	}
	if (std::strtod(_headers[CONTENT_LENGTH].c_str(), 0) > _body.size()){
		_errors.push_back(BODY_LENGTH_NOT_MATCHING_CONTENT_LENGTH);
		_validity = INCOMPLETE_REQUEST;
	}
	if (std::strtod(_headers[CONTENT_LENGTH].c_str(), 0) < _body.size()){
		_errors.push_back(BODY_LENGTH_NOT_MATCHING_CONTENT_LENGTH);
		_validity = INVALID_REQUEST;
	}
}

void HttpRequest::logLexerParserError(HttpRequest::ERRORS validity) {
	switch (validity) {
		case RAW_MESSAGE_TOO_SHORT:
			coloredLog("Raw message too short : ", _rawMessage, RED);
			break;
		case NO_CLRF_FOUND:
			coloredLog("No CRLF found in raw message : ", _rawMessage, RED);
			break;
		case NO_CLRFCLRF_FOUND:
			coloredLog("No CLRFCLRF found in raw message : ", _rawMessage, RED);
			break;
		case REQUEST_LINE_INVALID:
			coloredLog("Request line invalid : ", _rawMessage, RED);
			break;
		case ALL_LEXER_TOKENS_VALID:
			coloredLog("Lexer tokens valid", "", GREEN);
			break;
		case UNSUPPORTED_METHOD:
			coloredLog("Unsupported method : ", "\"" + _method + "\"", RED);
			break;
		case REQUEST_URI_NOT_ALNUM:
			coloredLog("Request URI not alphanumeric : ", "\"" + _requestUri + "\"", RED);
			break;
		case REQUEST_URI_FORBIDDEN_SYNTAX:
			coloredLog("Request URI : \"..\" or \"//\" forbidden : ", "\"" + _requestUri + "\"", RED);
			break;
		case INVALID_HTTP_VERSION:
			coloredLog("Invalid HTTP version : ", "\"" + _httpVersion + "\"", RED);
			break;
		case NO_COLON_FOUND_IN_HEADER:
			coloredLog("No colon found in header : ", "\"" + _rawMessage + "\"", RED);
			break;
		case NO_VALUE_FOUND_FOR_HEADER:
			coloredLog("No value found for header : ", "\"" + _rawMessage + "\"", RED);
			break;
		case BODY_WITHOUT_CONTENT_LENGTH:
			coloredLog("Body without content length : ", "\"" + _rawMessage + "\"", RED);
			break;
		case BODY_LENGTH_NOT_MATCHING_CONTENT_LENGTH:
			coloredLog("Body length not matching content length : ",
					   "\nBody size : " +
					   toString(_body.size()) +
					   "\nContent-Length : " +
					   _headers[CONTENT_LENGTH], RED);
			break;
	}
}

const std::string &HttpRequest::getRequestUri() {
	return _requestUri;
}

const HttpRequest::REQUEST_VALIDITY &HttpRequest::getValidity() const {
	return _validity;
}

void HttpRequest::logErrors() {
	std::vector<HttpRequest::ERRORS>::iterator it;
	for (it = _errors.begin(); it != _errors.end(); it++) {
		logLexerParserError(*it);
	}
	_errors.clear();
}