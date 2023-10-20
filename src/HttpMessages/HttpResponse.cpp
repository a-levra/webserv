#include <sys/stat.h>
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
	VirtualServer *vs  = server.getVirtualServer(host);
	if (vs == NULL){
		buildErrorPage(404, server);
		return _rawMessage;
	}
	_path = request.getPath();
	Location *loc = vs->getLocation(_path);
	if (loc == NULL){
		buildErrorPage(404, server);
		return _rawMessage;
	}
	this->build(*loc, host);
	return _rawMessage;
}

void HttpResponse::build(Location &location, std::string host, const HttpRequest &request) {
	(void) host; //todo: use host to get the right server name

	if ( !location.isAllowedMethod(request.getMethod()) ){
		coloredLog("Method not allowed: ", request.getMethod(), RED);
		this->buildErrorPage(405);
		return ;
	}
	coloredLog("Method allowed: ", "\"" + request.getMethod() + "\"", GREEN);
	if (request.getMethod() == "GET")
		this->buildGet(location);
	else if (request.getMethod() == "POST")
		this->buildPost(location, request);
//	else if (this->_method == "DELETE") //todo implement delete
//		this->buildDelete(location);
}

void HttpResponse::buildGet(Location &location) {
	std::string response;
	coloredLog("Building GET response: ", "", GREEN);
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

	const std::string *index = getFirstValidIndex(location);

	if (index == NULL){
		coloredLog("No index valid: ", _path, RED);
		this->buildErrorPage(500);
		return ;
	}
	_body = readFileToString( location.getRoot() + _path + "/" + *index );
	if (_body.empty()){
		this->buildErrorPage(500);
		return ;
	}
	else {
		this->setStatusCode(200);
		this->setStatusMessage("OK");
		this->setBody(_body);
		return ;
	}
}

void HttpResponse::buildPost(Location &location, const HttpRequest &request) {
	getFileFromPostAndSaveIt(request);
	this->buildGet(location);
}

void HttpResponse::buildErrorPage(int i) {
	std::string response;
	setStatusCode(i);
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
	std::string body = "<html><body><h1>" + error + " " + errorName + "</h1></body></html>";
	response += "Content-Length: " + toString(body.length()) + "\r\n";
	response += "\r\n";
	response += body;
	_rawMessage = response;
}

void HttpResponse::setHeader(std::string header, std::string content) {
	_headers[header] = content;
}

const std::string * HttpResponse::getFirstValidIndex(const Location &location) const {
	const std::vector<std::string> &index = location.getIndex();
	std::vector<std::string>::const_iterator it;
	for (it = index.begin(); it != index.end(); it++) {
		std::string pathToFile = location.getRoot() + "/" + _path;
		pathToFile += *it;
		if (fileExists(pathToFile)){
			coloredLog("Index found: ", pathToFile, GREEN);
			return &(*it);
		}
	}
	return NULL;
}

bool HttpResponse::fileExists(const std::string &pathToFile) const {
	struct stat buffer;
	return (stat(pathToFile.c_str(), &buffer) == 0);
void HttpResponse::getFileFromPostAndSaveIt(HttpRequest request) {
	coloredLog("Building POST response: ", "", BLUE);
	std::string *boundary = request.getHeader("Content-Type");
	*boundary = boundary->substr(boundary->find("boundary=") + 9);
	std::string body = request.getBody();
	std::string filename = body.substr(body.find("filename=") + 10);
	filename = filename.substr(0, filename.find("\r\n"));
	std::string fileContent = body.substr(body.find(*boundary) + boundary->length() + 2);
	fileContent = fileContent.substr(0, fileContent.find(*boundary) - 2);
	std::ofstream file;
	file.open(filename.c_str());
	file << fileContent;
	file.close();

}
