#include "HttpMessages/AHttpMessage.hpp"

#include <iostream>
#include <exception>

#include "utils/utils.hpp"
#include "logger/logging.hpp"

AHttpMessage::AHttpMessage(void) : _statusCode(-1) {}

AHttpMessage::AHttpMessage(std::string raw) : _statusCode(-1) {
	setRawRequest(raw);
}

AHttpMessage::AHttpMessage(const AHttpMessage &other) { *this = other; }

AHttpMessage::~AHttpMessage(void) {}

AHttpMessage &AHttpMessage::operator=(const AHttpMessage &other) {
	if (this != &other) {
		_method = other._method;
		_requestUri = other._requestUri;
		_httpVersion = other._httpVersion;
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
	logging::debug("\"" + message + "\"");
}

void AHttpMessage::displayRequest() {
	logging::debug("Method: " + _method);
	logging::debug("Path: " + _requestUri);
	logging::debug("Version: " + _httpVersion);
	logging::debug("Headers:");
	std::map<std::string, std::string>::iterator it;
	for (it = _headers.begin(); it != _headers.end(); it++) {
		logging::debug("\t" + it->first + ": " + it->second);
	}
//	logging::debug("Body: " + _body);
//	logging::debug("Raw request: " + _rawMessage);
}

void AHttpMessage::setVersion() {
	_httpVersion = HTTP_VERSION;
}

void AHttpMessage::setStatusCode(int code) {
	_statusCode = code;
}

int AHttpMessage::getStatusCode() {
	return _statusCode;
}

void AHttpMessage::setStatusMessage(std::string statusMessage) {
	_statusMessage = statusMessage;
}

void AHttpMessage::setHeaders(std::string header, std::string content) {
	_headers[header] = content;
}

void AHttpMessage::setBody(std::string body) {
	_body = body;
}

std::string AHttpMessage::getRawRequest() {
	return _rawMessage;
}

const std::map<std::string, std::string>& AHttpMessage::getHeaders() const
{
	return _headers;
}

std::string * AHttpMessage::getHeader(const std::string &header)  {
	std::map<std::string, std::string>::iterator it;
	for (it = _headers.begin(); it != _headers.end(); it++) {
		if (it->first == header)
			return &(it->second);
	}
	return NULL;
}

void AHttpMessage::appendBody(const std::string &message) {
	_body += message;
}

const std::string & AHttpMessage::getMethod() const {
	return _method;
}

const std::string &AHttpMessage::getBody() const {
	return _body;
}
