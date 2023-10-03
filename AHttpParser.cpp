#include <iostream>

#include "AHttpParser.hpp"
#include <exception>

#define MAX_METHOD_LENGTH 6 // "DELETE" is the longest (supported) method

AHttpParser::AHttpParser(void) : _isValid(true) {}

AHttpParser::AHttpParser(const AHttpParser &other) { *this = other; }

AHttpParser::~AHttpParser(void) {}

AHttpParser &AHttpParser::operator=(const AHttpParser &other) {
	if (this != &other) {

	}
	return (*this);
}

void AHttpParser::parse() {
	parseMethod();
	parsePath();
	parseVersion();
	parseHeaders();
//	parseBody();
//	checkValidity();
}

void AHttpParser::parseMethod() {
	size_t spacePos = _rawRequest.substr(0, MAX_METHOD_LENGTH + 1).find(' ');

	if (spacePos <= MAX_METHOD_LENGTH)
		_method = _rawRequest.substr(0, spacePos);
	else {
		_isValid = false;
		throw InvalidRequestException();
	}

	if (_method != "GET" &&
		_method != "POST" &&
		_method != "DELETE") {
		_isValid = false;
		throw InvalidRequestException();
	}
	_rawRequest = _rawRequest.substr(spacePos + 1);
}

void AHttpParser::parsePath() {
	size_t spacePos = _rawRequest.find(' ');
	if (spacePos == std::string::npos) {
		_isValid = false;
		throw InvalidRequestException();
	}
	checkPathValidity(spacePos);
	_path = _rawRequest.substr(0, spacePos);
	_rawRequest = _rawRequest.substr(spacePos + 1);
}

void AHttpParser::checkPathValidity(size_t spacePos) {
	//path must be alphanumeric and can contain only '/', '.' and '-'
	//path cannot contain ".." or "//"
	for (size_t i = 0; i < spacePos; i++) {
		if ((!isalnum(_rawRequest[i]) && _rawRequest[i] != '/' && _rawRequest[i] != '.') ||
			((_rawRequest[i] == '.' && _rawRequest[i + 1] == '.')) ||
			(_rawRequest[i] == '/') && (_rawRequest[i + 1] == '/')) {
			throw InvalidPathException();
		}
	}
}

void AHttpParser::parseVersion() {
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

void AHttpParser::parseHeaders() {
	std::string line;
	size_t crlfPos = _rawRequest.find("\r\n");

	if (crlfPos == std::string::npos) {
		_isValid = false;
		throw InvalidRequestException();
	}
	line = _rawRequest.substr(0, crlfPos);
	_rawRequest = _rawRequest.substr(crlfPos + 2);
	while (!line.empty()) {
		processLine(line);
//		display(line);

		crlfPos = _rawRequest.find("\r\n");
		line = _rawRequest.substr(0, crlfPos);
		_rawRequest = _rawRequest.substr(crlfPos + 2);
	}
	display(_rawRequest);
}

void AHttpParser::processLine(const std::string &line) {
	size_t semicolPos = line.find(':');
	if (semicolPos == std::string::npos) {
		_isValid = false;
		throw InvalidRequestException();
	}
	_headers[line.substr(0, semicolPos)] = line.substr(semicolPos + 2);
}

void AHttpParser::setRawRequest(const char *string) {
	_rawRequest = string;
}

void AHttpParser::display(std::string message) {
	std::cout << "\"" << message << "\"" << std::endl;
}

void AHttpParser::displayRequest() {
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
