#include <iostream>
#include <exception>

#include "HttpMessages/AHttpMessage.hpp"
#include "utils/utils.hpp"


AHttpMessage::AHttpMessage(void) : _statusCode(-1) {}

AHttpMessage::AHttpMessage(std::string raw) : _statusCode(-1) {
	this->setRawRequest(raw);
}

AHttpMessage::AHttpMessage(const AHttpMessage &other) { *this = other; }

AHttpMessage::~AHttpMessage(void) {}

AHttpMessage &AHttpMessage::operator=(const AHttpMessage &other) {
	if (this != &other) {
		_method = other._method;
		_path = other._path;
		_version = other._version;
		_headers = other._headers;
		_body = other._body;
		_rawMessage = other._rawMessage;
	}
	return (*this);
}

void AHttpMessage::setRawRequest(std::string string) {
	_rawMessage = string;
}

void AHttpMessage::display(std::string message) {
	std::cout << "\"" << message << "\"" << std::endl;
}

void AHttpMessage::displayRequest() {
	printBoldThenThin("Method: ", _method);
	printBoldThenThin("Path: ", _path);
	printBoldThenThin("Version: ", _version);
	printBoldThenThin("Headers:", "");

	std::map<std::string, std::string>::iterator it;
	for (it = _headers.begin(); it != _headers.end(); it++) {
		printBoldThenThin("\t" + it->first + ": ", it->second);
	}
	printBoldThenThin("Body: ", _body);
	printBoldThenThin("Raw request: ", _rawMessage);
}

void AHttpMessage::setVersion() {
	this->_version = "HTTP/1.1";
}

void AHttpMessage::setStatusCode(int code) {
	this->_statusCode = code;
}

void AHttpMessage::setStatusMessage(std::string statusMessage) {
	this->_statusMessage = statusMessage;
}

void AHttpMessage::setHeaders(std::string header, std::string content) {
	this->_headers[header] = content;
}

void AHttpMessage::setBody(std::string body) {
	this->_body = body;
}

std::string AHttpMessage::getRawRequest() {
	return _rawMessage;
}

void AHttpMessage::setBodyFromFile(std::string path) {
	//open a file, and put its content in _body
	setBody(readFileToString(path));
}

std::string AHttpMessage::getHeader(std::string header) {
	std::map<std::string, std::string>::iterator it;
	for (it = _headers.begin(); it != _headers.end(); it++) {
		if (it->first == header)
			return it->second;
	}
	return "";
}
