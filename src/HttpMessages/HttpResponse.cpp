#include <fstream>

#include "HttpMessages/HttpResponse.hpp"
#include "utils/utils.hpp"

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
		return buildErrorPage(400);

	const std::string *host = _request.getHeader("Host");
	if (host == NULL)
		return buildErrorPage(400);

	coloredLog("Host requested: ", *host, PURPLE);
	VirtualServer *vs  = server.getVirtualServer(*host);
	if (vs == NULL)
		return buildErrorPage(404);

	coloredLog("Virtual server found: ", vs->getServerName()[0], GREEN);
	coloredLog("URI requested: ", _requestUri, PURPLE);
	_requestUri = _request.getRequestUri();
	Location *loc = vs->getLocation(_requestUri);
	if (loc == NULL)
		return buildErrorPage(404);
	coloredLog("Location best match: ", "", GREEN);
	loc->display();
	this->build(*loc, *host);
	return _rawMessage;
}

void HttpResponse::build(Location &location, const std::string& host) {
	(void) host; //todo: use host to get the right server name

	if ( !location.isAllowedMethod(_request.getMethod()) ){
		buildErrorPage(405);
		return ;
	}
	coloredLog("Method allowed: ", "\"" + _request.getMethod() + "\"", GREEN);
	if (_request.getMethod() == "GET")
		this->buildGet(location);
	else if (_request.getMethod() == "POST")
		this->buildPost(location);
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

	response += HTTP_VERSION  " " + toString(this->_statusCode) + CRLF;
	std::map<std::string, std::string>::iterator it;
	for (it = _headers.begin(); it != _headers.end(); it++) {
		response += it->first + ": " + it->second + CRLF;
	}
	response += CRLF;
	response += _body;
	_rawMessage = response;
}

void HttpResponse::generateBody(Location &location) {

	const std::string *index = getFirstValidIndex(location);

	if (index == NULL){
		coloredLog("No index valid: ", _requestUri, RED);
		this->buildErrorPage(500);
		return ;
	}
	_body = readFileToString( location.getRoot() + location.getURI() + "/" + *index );
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

void HttpResponse::buildPost(Location &location) {
	getFileFromPostAndSaveIt();
	this->buildGet(location);
}

std::string & HttpResponse::buildErrorPage(int i) {
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

void HttpResponse::setHeader(std::string header, std::string content) {
	_headers[header] = content;
}

const std::string * HttpResponse::getFirstValidIndex(const Location &location) const {
	const std::vector<std::string> &index = location.getIndex();
	std::vector<std::string>::const_iterator it;
	for (it = index.begin(); it != index.end(); it++) {
		std::string pathToFile = location.getRoot() + location.getURI() + "/" + *it;
		coloredLog("Index tested: ", pathToFile, YELLOW);
		if (fileExists(pathToFile)){
			coloredLog("Index found: ", pathToFile, GREEN);
			return &(*it);
		}
	}
	coloredLog("No index found: ", location.getURI(), RED);
	return NULL;
}

void HttpResponse::GenerateErrorBody() {
	coloredLog("Error page generated: ", "", RED);
	coloredLog("Error code: ", toString(_statusCode), RED);
	coloredLog("Error message: ", _statusMessage, RED);
	setBody(GENERIC_CSS_STYLE);
	appendBody("<html>"
					"<body>"
							 "<h1>\n" +toString(_statusCode) + " " + 	_statusMessage + "\n</h1>" \
							 "<h2>\n" + _request.getMethod() + " " + _request.getRequestUri() + "\n</h2>" \
							 "<h3>\n" + _request.getErrors() + "\n</h3>" \
					"</body></html>");

}

void HttpResponse::getFileFromPostAndSaveIt() {
	coloredLog("Building POST response: ", "", BLUE);

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
			this->buildErrorPage(500);
		}
	}
	createFile(filename, fileContent);
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
