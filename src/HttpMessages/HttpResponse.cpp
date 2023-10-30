#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>

#include "HttpMessages/HttpResponse.hpp"
#include "utils/utils.hpp"
#include "logger/logging.hpp"

HttpResponse::HttpResponse(class HttpRequest& r) : _request(r) {}

HttpResponse::HttpResponse(const HttpResponse &other) : AHttpMessage(other), _request(other._request)
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
std::string HttpResponse::getResponse(Server &server) {

	if (_request.isInvalid())
		return buildErrorPage(BAD_REQUEST);

	const std::string *host = _request.getHeader("Host");
	if (host == NULL)
		return buildErrorPage(BAD_REQUEST);

	coloredLog("Host requested: ", *host, PURPLE);
	VirtualServer *vs  = server.getVirtualServer(*host);
	if (vs == NULL)
		return buildErrorPage(NOT_FOUND);

	coloredLog("Virtual server found: ", vs->getServerName()[0], GREEN);
	_requestUri = _request.getRequestUri();
	coloredLog("URI requested: ", _requestUri, PURPLE);
	_location = vs->getLocation(_requestUri);
	if (_location == NULL)
		return buildErrorPage(NOT_FOUND);
	coloredLog("Location best match: ", "", GREEN);
	_location->display();
	this->build();
	return _rawMessage;
}

void HttpResponse::build() {

	if ( !_location->isAllowedMethod(_request.getMethod()) ){
		buildErrorPage(METHOD_NOT_ALLOWED);
		return ;
	}
	coloredLog("Building...", "\"" + _request.getMethod() + "\"", BLUE);
	if (!checkRequestMaxBodySize()) {
		this->buildErrorPage(PAYLOAD_TOO_LARGE);
		return;
	}
	if (_request.getValidity() == HttpRequest::VALID_AND_INCOMPLETE_REQUEST){
		_rawMessage = "";
		return;
	}

	if (_request.getMethod() == "GET")
		this->buildGet();
	else if (_request.getMethod() == "POST")
		this->buildPost();
	else if (_request.getMethod() == "DELETE")
		this->buildDelete();
	else
		this->buildErrorPage(NOT_IMPLEMENTED);
}

bool HttpResponse::checkRequestMaxBodySize() {
	if ((float)_request.getBody().size() / 1000000 > (float)_location->getClientMaxBodySize()){
		logging::debug("Payload to large : ");
		logging::debug("\tBody size : " + toString(_request.getBody().size() / 1000000) + "m");
		logging::debug("Max body size : " + toString(_location->getClientMaxBodySize()) + "m");
		_request.addError(HttpRequest::PAYLOAD_TOO_LARGE_ERROR);
		return false;
	}
	return true;
}

void HttpResponse::buildGet() {
	std::string response;
	coloredLog("Building GET response: ", "", GREEN);

	this->generateBody();
	this->setHeader("Date", getDate());
	this->setHeader("Server", "webserv");
	this->setHeader("Content-Length", toString(_body.length()));

	response += HTTP_VERSION  " " + toString(this->_statusCode) + CRLF;
	std::map<std::string, std::string>::iterator it;
	for (it = _headers.begin(); it != _headers.end(); it++) {
		response += it->first + ": " + it->second + CRLF;
	}
	response += CRLF;
	response += _body;
	_rawMessage = response;
}

void HttpResponse::generateBody() {
	const std::string *file;

	std::string res = getResource();
	if (!res.empty()){
		file = tryGetFile(res);
		if (file == NULL){
			this->buildErrorPage(404);
			return ;
		}
	}
	else
		file = getFirstValidIndex();

	if (file == NULL){
		coloredLog("No index valid: ", _requestUri, RED);
		this->buildErrorPage(500);
		return ;
	}
	_body = readFileToString( _location->getRoot() + _location->getURI() + "/" + *file );
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

void HttpResponse::buildPost() {
	getFileFromPostAndSaveIt();
	updateHTML();
	this->buildGet();
}

void HttpResponse::buildDelete() {
	logging::debug("Building DELETE response: ");
	_request.displayRequest();
	std::string res = getResource();
	logging::debug("Resource: " + res);
	std::string file = "/app/uploadedFiles" + res;
	if (!fileExists(file))
		this->buildErrorPage(404);
	else if (std::remove(file.c_str()) != 0)
		this->buildErrorPage(500);
	updateHTML();
}

std::string & HttpResponse::buildErrorPage(int errorCode) {
	std::string response;
	setStatusCode(errorCode);
	std::string error = toString(errorCode);
	std::string errorName;
	switch (errorCode) {
		case BAD_REQUEST:
			errorName = "Bad Request";
			break;
		case NOT_FOUND:
			errorName = "Not Found";
			break;
		case METHOD_NOT_ALLOWED:
			errorName = "Method Not Allowed";
			break;
		case PAYLOAD_TOO_LARGE:
			errorName = "Payload Too Large";
			break;
		case INTERNAL_SERVER_ERROR:
			errorName = "Internal Server Error";
			break;
		case NOT_IMPLEMENTED:
			errorName = "Not Implemented";
			break;
		case HTTP_VERSION_NOT_SUPPORTED:
			errorName = "HTTP Version Not Supported";
			break;
		default:
			errorName = "Unknown Error";
			break;
	}
	setStatusMessage(errorName);
	response += HTTP_VERSION " " + error + " " + errorName + CRLF;
	response += "Content-Type: text/html" CRLF;
	this->GenerateErrorBody();
	response += "Content-Length: " + toString(_body.length()) + CRLF;
	response += CRLF;
	response += _body;
	_rawMessage = response;
	return _rawMessage;
}

void HttpResponse::setHeader(const std::string& header, const std::string& content) {
	_headers[header] = content;
}

const std::string * HttpResponse::getFirstValidIndex() const {
	if (!_location)
		return NULL;
	const std::vector<std::string> &index = _location->getIndex();
	std::vector<std::string>::const_iterator it;
	for (it = index.begin(); it != index.end(); it++) {
		std::string pathToFile = _location->getRoot() + _location->getURI() + "/" + *it;
		coloredLog("Index tested: ", pathToFile, YELLOW);
		if (fileExists(pathToFile)){
			coloredLog("Index found: ", pathToFile, GREEN);
			return &(*it);
		}
	}
	coloredLog("No index found: ", _location->getURI(), RED);
	return NULL;
}

void HttpResponse::GenerateErrorBody() {
	logging::debug("Error page generated: ");
	logging::debug("Error code: " + toString(_statusCode));
	logging::debug("Error message: " + _statusMessage);
	std::string additionnalInfo;
	if (_statusCode == PAYLOAD_TOO_LARGE){
		additionnalInfo = "The maximum size of the payload for \"" + _location->getURI() + "\" is " + toString(this->_location->getClientMaxBodySize()) + " Mb.";
		additionnalInfo += "</h3><h3>Current payload size: " + toString(_request.getBody().size() / 1000000) + " Mb.";
		if (_request.getHeader(CONTENT_LENGTH)){
			additionnalInfo += "</h3><h3>Total payload you tried to send: ";
			std::string payloadSizeStr = *(_request.getHeader(CONTENT_LENGTH));
			size_t payloadSize = std::strtod(payloadSizeStr.c_str(), 0);
			additionnalInfo += toString(payloadSize / 1000000) + " Mb.";
		}
	}
	else
		additionnalInfo = _request.getErrors();
	appendBody("<html>"
			  			 GENERIC_CSS_STYLE
					"<body>"
					NAVBAR
							 "<h1>\n" +toString(_statusCode) + " " + 	_statusMessage + "\n</h1>" \
							 "<h2>\n" + _request.getMethod() + " " + _request.getRequestUri() + "\n</h2>" \
							 "<h3>\n" + additionnalInfo + "\n</h3>" \
					"</body></html>");

}

void HttpResponse::getFileFromPostAndSaveIt() {
	logging::debug("Building POST response ");

	std::string *boundary = _request.getHeader("Content-Type");
	std::string filename = "file";
	std::string fileContent;
	if (boundary == NULL || boundary->find("boundary=") == std::string::npos)
		fileContent = _request.getBody();
	else{
		try {
			ExtractImgInsideBoundaries(boundary, filename, fileContent);
		}
		catch (std::exception &e) {
			coloredLog("Error: ", e.what(), RED);
			this->buildErrorPage(INTERNAL_SERVER_ERROR);
		}
	}
	if (!createFile(filename, fileContent)){
		this->buildErrorPage(500);
	}
}

void HttpResponse::ExtractImgInsideBoundaries(std::string *boundary,
											  std::string &filename,
											  std::string &fileContent) const {
	coloredLog("Boundary found: ", *boundary, BLUE);
	*boundary = boundary->substr(boundary->find("boundary=") + 9);
	std::string body = _request.getBody();

	filename = body.substr(body.find("filename=") + 9);
	char delimiter = filename[0];
	filename = filename.substr(1, filename.substr(1).find(delimiter));
	coloredLog("File name: ", filename, BLUE);

	fileContent = body.substr(body.find(*boundary) + boundary->length() + 2);
	//trim all headers
	fileContent = fileContent.substr(fileContent.find(CRLF CRLF) + 4);
	fileContent = fileContent.substr(0, fileContent.find(*boundary) - 2);
	if (fileContent.size() >= 2 && fileContent[fileContent.size() - 2] == '\r' && fileContent[fileContent.size() - 1] == '\n')
		fileContent.erase(fileContent.size() - 2, 2);
}

std::string HttpResponse::getResource() const {
	std::string resource;
	if (_location)
		resource = _requestUri.substr(_location->getURI().length());
	return resource;
}

const std::string *HttpResponse::tryGetFile(const std::string &resource) {
	if (!_location)
		return NULL;
	std::string pathToFile = _location->getRoot() + _location->getURI() + "/" + resource;
	coloredLog("pathToFile: ", pathToFile, YELLOW);
	if (fileExists(pathToFile))
		return &resource;
	coloredLog("File not found: ", pathToFile, RED);
	return NULL;
}


