#include <iostream>
#include <exception>

#include "HttpRequest.hpp"
#include "utils.hpp"

#define MAX_METHOD_LENGTH 6 // "DELETE" is the longest (supported) method

HttpRequest::HttpRequest(void) : _statusCode(-1) {}

HttpRequest::HttpRequest(std::string raw) : _statusCode(-1) {
	this->setRawRequest(raw);
}

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
	}
	return (*this);
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
			break;
		}
	}
	return success;
//	parseBody();
}

bool HttpRequest::parseMethod() {
	size_t spacePos = _rawRequest.substr(0, MAX_METHOD_LENGTH + 1).find(' ');

	if (spacePos <= MAX_METHOD_LENGTH)
		_method = _rawRequest.substr(0, spacePos);
	else
		return false;

	if (_method != "GET" &&
		_method != "POST" &&
		_method != "DELETE")
		return false;
	_rawRequest = _rawRequest.substr(spacePos + 1);
	return true;
}

bool HttpRequest::parsePath() {
	size_t spacePos = _rawRequest.find(' ');
	if (spacePos == std::string::npos || !checkPathValidity(spacePos))
		return false;
	_path = _rawRequest.substr(0, spacePos);
	_rawRequest = _rawRequest.substr(spacePos + 1);
	return true;
}

bool HttpRequest::checkPathValidity(size_t spacePos) {
	//path must be alphanumeric and can contain only '/', '.' and '-'
	//path cannot contain ".." or "//"
	for (size_t i = 0; i < spacePos; i++) {
		if (
			(!isalnum(_rawRequest[i]) && _rawRequest[i] != '/' && _rawRequest[i] != '.') ||
				(_rawRequest[i] == '.' && _rawRequest[i + 1] == '.') ||
				((_rawRequest[i] == '/') && (_rawRequest[i + 1] == '/')))
			return false;
	}
	return true;
}

bool HttpRequest::parseVersion() {
	size_t crlfPos = _rawRequest.find("\r\n");
	if (crlfPos == std::string::npos)
		return false;
	_version = _rawRequest.substr(0, crlfPos);
	if (_version != "HTTP/1.1")
		return false;
	_rawRequest = _rawRequest.substr(crlfPos + 2);
	return true;
}

bool HttpRequest::parseAllHeaders() {
	std::string line;
	size_t crlfPos = _rawRequest.find("\r\n");

	if (crlfPos == std::string::npos)
		return false;
	line = _rawRequest.substr(0, crlfPos);
	_rawRequest = _rawRequest.substr(crlfPos + 2);
	while (!line.empty()) {
		if (!parseHeader(line))
			return false;
		crlfPos = _rawRequest.find("\r\n");
		line = _rawRequest.substr(0, crlfPos);
		_rawRequest = _rawRequest.substr(crlfPos + 2);
	}
	display(_rawRequest);
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

void HttpRequest::setRawRequest(std::string string) {
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
}

bool HttpRequest::checkDoubleSpaces() {
	size_t pos = _rawRequest.find("  ");
	if (pos != std::string::npos)
		return false;
	return true;
}

void HttpRequest::setVersion() {
	this->_version = "HTTP/1.1";
}

void HttpRequest::setStatusCode(int code) {
	this->_statusCode = code;
}

void HttpRequest::setStatusMessage(std::string statusMessage) {
	this->_statusMessage = statusMessage;
}

void HttpRequest::setHeaders(std::string header, std::string content) {
	this->_headers[header] = content;
}

void HttpRequest::setBody(std::string body) {
	this->_body = body;
}

void HttpRequest::build() {
	std::string request = "";
	request += _version + " " + toString(_statusCode) + " " + _statusMessage + "\r\n";
	std::map<std::string, std::string>::iterator it;
	for (it = _headers.begin(); it != _headers.end(); it++) {
		request += it->first + ": " + it->second + "\r\n";
	}
	request += "\r\n";
	request += _body;
	_rawRequest = request;
}

std::string HttpRequest::getRawRequest() {
	return _rawRequest;
}

void HttpRequest::setBodyFromFile(std::string path) {
	//open a file, and put its content in _body
	setBody(readFileToString(path));
}