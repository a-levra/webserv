#include "HttpMessages/HttpResponse.hpp"

#define HTTP_VERSION "HTTP/1.1 "

HttpResponse::HttpResponse(void) {}

HttpResponse::HttpResponse(const HttpResponse &other) : AHttpMessage(other)
{ *this = other; }

HttpResponse::~HttpResponse(void) {}

HttpResponse &HttpResponse::operator=(const HttpResponse &other) {
	if (this != &other) {
		AHttpMessage::operator=(other);
	}
	return (*this);
}

void HttpResponse::build() {
//	std::string response;
//
//	this->generateBody();
//	this->setHeader("Date", getDate());
//	this->setHeader("Server", "webserv");
//	this->setHeader("Content-Length", toString(_body.length()));
//	this->setStatusCode(200);
//	this->setStatusMessage("OK");
//
//	response += HTTP_VERSION + toString(this->_statusCode) + "\r\n";
//	std::map<std::string, std::string>::iterator it;
//	for (it = _headers.begin(); it != _headers.end(); it++) {
//		response += it->first + ": " + it->second + "\r\n";
//	}
//	response += "\r\n";
//	response += getBody();
//
//	_rawMessage = response;
}

/*
 * This function is called by the server when a request is received.
 *  1) parse the host inside the request
 *  2) get the virtual server corresponding to the host
 *  3) get the location corresponding to the path and the host
 *  4) build the response into _rawMessage and returns it
 */
std::string HttpResponse::getResponse(Server &server, HttpRequest &request) {
	std::string host = request.getHeader("Host");
	coloredLog("Host requested: ", host, PURPLE);
	server.displayVirtualServers();
	VirtualServer *vs  = server.getVirtualServer(host);
	if (vs == NULL){
		buildErrorPage(404, server);
		return _rawMessage;
	}
	_path = request.getPath();
//	Location *loc = vs->getLocation(_path); //todo check if loc is null
//	loc->display();
//	this->build(*loc, host);
	return _rawMessage;
}

void HttpResponse::buildErrorPage(int i, Server &server) {
	(void) server;
	std::string response;
	std::string error = toString(i);
	std::string errorName;
	switch (i) {
		case 400:
			errorName = "Bad Request";
			break;
		case 403:
			errorName = "Forbidden";
			break;
		case 404:
			errorName = "Not Found";
			break;
		case 405:
			errorName = "Method Not Allowed";
			break;
		case 413:
			errorName = "Payload Too Large";
			break;
		case 500:
			errorName = "Internal Server Error";
			break;
		case 501:
			errorName = "Not Implemented";
			break;
		case 505:
			errorName = "HTTP Version Not Supported";
			break;
		default:
			errorName = "Unknown Error";
			break;
	}
	response += HTTP_VERSION + error + " " + errorName + "\r\n";
	response += "Content-Type: text/html\r\n";
	response += "\r\n";
	response += "<html><body><h1>" + error + " " + errorName + "</h1></body></html>";
	_rawMessage = response;
}

void HttpResponse::setHeader(std::string header, std::string content) {
	_headers[header] = content;
}

void HttpResponse::build(Location &location, std::string host) {
	(void) host;
	std::string response;
	this->generateBody(location);
	this->setHeader("Date", getDate());
	this->setHeader("Server", "webserv");
	this->setHeader("Content-Length", toString(_body.length()));

	response += HTTP_VERSION + toString(this->_statusCode) + "\r\n";
	std::map<std::string, std::string>::iterator it;
	for (it = _headers.begin(); it != _headers.end(); it++) {
		response += it->first + ": " + it->second + "\r\n";
	}
	response += "\r\n";
	response += _body;

	_rawMessage = response;
}

void HttpResponse::generateBody(Location &location) {
	_body = readFileToString("./assets/" + location.getRoot() + "/" + _path + "/index.html");
	if (_body.empty()){
		this->setStatusCode(500);
		this->setStatusMessage("Internal Server Error");
		this->setBody("Internal Server Error");
		return ;
	}
	else {
		this->setStatusCode(200);
		this->setStatusMessage("OK");
		this->setBody(_body);
		return ;
	}
}
