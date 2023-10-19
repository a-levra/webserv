#include "HttpMessages/HttpRequest.hpp"

#include <iostream>
#include <stdlib.h>

#define MAX_METHOD_LENGTH 6 // "DELETE" is the longest (supported) method

HttpRequest::HttpRequest(void) {}

HttpRequest::HttpRequest(const std::string &raw) : AHttpMessage(raw) {
	this->parse();
}

HttpRequest::HttpRequest(const HttpRequest &other) : AHttpMessage(other) { *this = other; }

HttpRequest::~HttpRequest(void) {}

HttpRequest &HttpRequest::operator=(const HttpRequest &other) {
	if (this != &other) {
		_method = other._method;
		_path = other._path;
		_version = other._version;
		_headers = other._headers;
		_body = other._body;
		_rawMessage = other._rawMessage;
		_statusCode = other._statusCode;
		_statusMessage = other._statusMessage;
	}
	return (*this);
}

bool HttpRequest::checkDoubleSpaces() {
	size_t pos = _rawMessage.find("  ");
	if (pos != std::string::npos)
		return false;
	return true;
}

bool HttpRequest::parse() {
	bool success = true;
	bool (HttpRequest::*parseFunctions[])() = {
		&HttpRequest::checkDoubleSpaces,
		&HttpRequest::parseMethod,
		&HttpRequest::parsePath,
		&HttpRequest::parseVersion,
		&HttpRequest::parseAllHeaders};

	unsigned long size = sizeof(parseFunctions) / sizeof(parseFunctions[0]);
	for (size_t i = 0; i < size; i++) {
		if (!(this->*parseFunctions[i])()) {
			success = false;
			std::cout << "parse failed at " << i << std::endl;
			break;
		}
	}
	_body = _rawMessage;
	_validity = checkValidity();
	return success;
}

bool HttpRequest::parseMethod() {
	if (_rawMessage.empty())
		return false;
	size_t spacePos = _rawMessage.substr(0, MAX_METHOD_LENGTH + 1).find(' ');
	if (spacePos <= MAX_METHOD_LENGTH)
		_method = _rawMessage.substr(0, spacePos);
	else {
		coloredLog("Method too long: ", _method, RED);
		return false;
	}

	if (_method != "GET" &&
		_method != "POST" &&
		_method != "DELETE") {
		std::cerr << "Method not supported : \'" << _method << "\'" << std::endl;
		return false;
	}
	_rawMessage = _rawMessage.substr(spacePos + 1);
	return true;
}

bool HttpRequest::parsePath() {
	size_t spacePos = _rawMessage.find(' ');
	if (spacePos == std::string::npos || !checkPathValidity(spacePos))
		return false;
	_path = _rawMessage.substr(0, spacePos);
	_rawMessage = _rawMessage.substr(spacePos + 1);
	return true;
}

bool HttpRequest::checkPathValidity(size_t spacePos) {
	//path must be alphanumeric and can contain only '/', '.' and '-'
	//path cannot contain ".." or "//"
	for (size_t i = 0; i < spacePos; i++) {
		if (
			(!isalnum(_rawMessage[i]) && _rawMessage[i] != '/' && _rawMessage[i] != '.') ||
				(_rawMessage[i] == '.' && _rawMessage[i + 1] == '.') ||
				((_rawMessage[i] == '/') && (_rawMessage[i + 1] == '/')))
			return false;
	}
	return true;
}

bool HttpRequest::parseVersion() {
	size_t crlfPos = _rawMessage.find("\r\n");
	if (crlfPos == std::string::npos)
		return false;
	_version = _rawMessage.substr(0, crlfPos);
	if (_version != "HTTP/1.1")
		return false;
	_rawMessage = _rawMessage.substr(crlfPos + 2);
	return true;
}

bool HttpRequest::parseAllHeaders() {
	std::string line;
	size_t crlfPos = _rawMessage.find("\r\n");

	if (crlfPos == std::string::npos)
		return false;
	line = _rawMessage.substr(0, crlfPos);
	_rawMessage = _rawMessage.substr(crlfPos + 2);
	while (!line.empty()) {
		if (!parseHeader(line))
			return false;
		crlfPos = _rawMessage.find("\r\n");
		line = _rawMessage.substr(0, crlfPos);
		_rawMessage = _rawMessage.substr(crlfPos + 2);
	}
	return true;
}

bool HttpRequest::parseHeader(const std::string &line) {
	size_t semicolPos = line.find(':');
	if (semicolPos == std::string::npos
		|| semicolPos == 0
		|| semicolPos == line.size() - 1
		|| line[semicolPos + 1] != ' '
		|| line[semicolPos + 2] == '\0'
		|| line[semicolPos + 2] == '\r'
		)
		return false;
	_headers[line.substr(0, semicolPos)] = line.substr(semicolPos + 2);
	return true;
}

const std::string &HttpRequest::getPath() {
	return _path;
}

enum REQUEST_VALIDITY HttpRequest::checkValidity() {
	if (_method.empty() || _path.empty() || _version.empty())
		return NOT_COMPLETE;
	if (_method != "GET" && _method != "POST" && _method != "DELETE")
		return INVALID;
	if (_version != "HTTP/1.1")
		return INVALID;
	if (_headers.find("Host") == _headers.end())
		return INVALID;
	if (_headers.find("Content-Length") != _headers.end() && _body.empty())
		return NOT_COMPLETE;
	if (_headers.find("Content-Length") == _headers.end() && !_body.empty())
		return INVALID;
	if (_headers.find("Content-Type") == _headers.end() && !_body.empty())
		return INVALID;
	if (_headers.find("Content-Type") != _headers.end() && _body.empty())
		return NOT_COMPLETE;
	if (_headers.find("Content-Length") != _headers.end() && !_body.empty()) {
		if (_headers["Content-Length"].size() > 10)
			return INVALID;
		for (size_t i = 0; i < _headers["Content-Length"].size(); i++) {
			if (!isdigit(_headers["Content-Length"][i]))
				return INVALID;
		}
		if (std::strtod(_headers["Content-Length"].c_str(), 0) != _body.size())
			return NOT_COMPLETE;
	}

	return VALID_and_COMPLETE;
}

const REQUEST_VALIDITY & HttpRequest::getValidity() const {
	return _validity;
}
