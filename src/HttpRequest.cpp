#include <iostream>

#include "HttpRequest.hpp"
#include <exception>

#define MAX_METHOD_LENGTH 6 // "DELETE" is the longest (supported) method

HttpRequest::HttpRequest(void) : _isValid(true) {}

HttpRequest::HttpRequest(const HttpRequest &other) { *this = other; }

HttpRequest::~HttpRequest(void) {}

HttpRequest &HttpRequest::operator=(const HttpRequest &other) {
	if (this != &other) {
		_method = other._method;
		_path = other._path;
		_version = other._version;
		_headers = other._headers;
		_body = other._body;
		_rawRequest = other._rawRequest;
		_isValid = other._isValid;
	}
	return (*this);
}

void HttpRequest::parse() {
	checkDoubleSpaces();
	parseMethod();
	parsePath();
	parseVersion();
	parseAllHeaders();
//	parseBody();
}

void HttpRequest::parseMethod() {
	size_t spacePos = _rawRequest.substr(0, MAX_METHOD_LENGTH + 1).find(' ');

	if (spacePos <= MAX_METHOD_LENGTH)
		_method = _rawRequest.substr(0, spacePos);
	else {
		_isValid = false;
		throw InvalidMethodException();
	}

	if (_method != "GET" &&
		_method != "POST" &&
		_method != "DELETE") {
		_isValid = false;
		throw InvalidMethodException();
	}
	_rawRequest = _rawRequest.substr(spacePos + 1);
}

void HttpRequest::parsePath() {
	size_t spacePos = _rawRequest.find(' ');
	if (spacePos == std::string::npos) {
		_isValid = false;
		throw InvalidRequestException();
	}
	checkPathValidity(spacePos);
	_path = _rawRequest.substr(0, spacePos);
	_rawRequest = _rawRequest.substr(spacePos + 1);
}

void HttpRequest::checkPathValidity(size_t spacePos) {
	//path must be alphanumeric and can contain only '/', '.' and '-'
	//path cannot contain ".." or "//"
	for (size_t i = 0; i < spacePos; i++) {
		if (
			(!isalnum(_rawRequest[i]) && _rawRequest[i] != '/' && _rawRequest[i] != '.') ||
			(_rawRequest[i] == '.' && _rawRequest[i + 1] == '.') ||
				((_rawRequest[i] == '/') && (_rawRequest[i + 1] == '/'))) {
			throw InvalidPathException();
		}
	}
}

void HttpRequest::parseVersion() {
	size_t crlfPos = _rawRequest.find("\r\n");
	if (crlfPos == std::string::npos) {
		_isValid = false;
		throw InvalidVersionException();
	}
	_version = _rawRequest.substr(0, crlfPos);
	if (_version != "HTTP/1.1") {
		_isValid = false;
		throw InvalidVersionException();
	}
	_rawRequest = _rawRequest.substr(crlfPos + 2);
}

void HttpRequest::parseAllHeaders() {
	std::string line;
	size_t crlfPos = _rawRequest.find("\r\n");

	if (crlfPos == std::string::npos) {
		_isValid = false;
		throw InvalidRequestException();
	}
	line = _rawRequest.substr(0, crlfPos);
	_rawRequest = _rawRequest.substr(crlfPos + 2);
	while (!line.empty()) {
		parseHeader(line);
//		display(line);

		crlfPos = _rawRequest.find("\r\n");
		line = _rawRequest.substr(0, crlfPos);
		_rawRequest = _rawRequest.substr(crlfPos + 2);
	}
	display(_rawRequest);
}

void HttpRequest::parseHeader(const std::string &line) {
	size_t semicolPos = line.find(':');
	if (semicolPos == std::string::npos
	|| semicolPos == 0
	|| semicolPos == line.size() - 1
	|| line[semicolPos + 1] != ' '
	|| line[semicolPos + 2] == '\0'
	|| line[semicolPos + 2] == '\r'
	) {
		_isValid = false;
		throw InvalidRequestException();
	}
	_headers[line.substr(0, semicolPos)] = line.substr(semicolPos + 2);
}

void HttpRequest::setRawRequest(const char *string) {
	_rawRequest = string;
}

void HttpRequest::display(std::string message) {
	std::cout << "\"" << message << "\"" << std::endl;
}

void HttpRequest::displayRequest() {
	std::cout << "Method: " << _method << std::endl;
	std::cout << "Path: " << _path << std::endl;
	std::cout << "Version: " << _version << std::endl;
	std::cout << "Headers:" << std::endl;
	std::map<std::string, std::string>::iterator it;
	for (it = _headers.begin(); it != _headers.end(); it++) {
		std::cout << "\t" << it->first << ": " << it->second << std::endl;
	}
	std::cout << "Body: " << _body << std::endl;
	std::cout << "Raw request: " << _rawRequest << std::endl;
	std::cout << "Is valid: " << _isValid << std::endl;
}

void HttpRequest::checkDoubleSpaces() {
	size_t pos = _rawRequest.find("  ");
	if (pos != std::string::npos) {
		_isValid = false;
		throw InvalidRequestException();
	}
}
