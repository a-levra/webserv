#include <fstream>

#include "HttpMessages/HttpResponse.hpp"
#include "utils/utils.hpp"

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
	const std::string *host = request.getHeader("Host");
	coloredLog("Host requested: ", *host, PURPLE);
	VirtualServer *vs  = server.getVirtualServer(*host);
	if (vs == NULL){
		coloredLog("Virtual server not found: ", *host, RED);
		buildErrorPage(404);
		return _rawMessage;
	}
	coloredLog("Virtual server found: ", vs->getServerName()[0], GREEN);
	_path = request.getPath();
	Location *loc = vs->getLocation(_path);
	coloredLog("Location requested: ", _path, PURPLE);
	if (loc == NULL){
		coloredLog("Location not found: ", _path, RED);
		buildErrorPage(404);
		return _rawMessage;
	}
	coloredLog("Location found: ", _path, GREEN);
	this->build(*loc, *host, request);
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

	std::string file;
	if (_file.empty()) {
		const std::string *index = getFirstValidIndex(location);
		if (index == NULL){
			coloredLog("No index valid: ", _path, RED);
			this->buildErrorPage(500);
			return ;
		}
		file = *index;
	}
	else
		file = _file;
	_body = readFileToString(location.getRoot() + _path + "/" + file);
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
	setStatusMessage(errorName);
	response += HTTP_VERSION + error + " " + errorName + "\r\n";
	response += "Content-Type: text/html\r\n";
	this->GenerateErrorBody();
	response += "Content-Length: " + toString(_body.length()) + "\r\n";
	response += "\r\n";
	response += _body;
	_rawMessage = response;
}

void HttpResponse::setHeader(std::string header, std::string content) {
	_headers[header] = content;
}

const std::string * HttpResponse::getFirstValidIndex(const Location &location) const {
	const std::vector<std::string> &index = location.getIndex();
	std::vector<std::string>::const_iterator it;
	for (it = index.begin(); it != index.end(); it++) {
		std::string pathToFile = location.getRoot() + _path + "/" + *it;
		coloredLog("Index tested: ", pathToFile, YELLOW);
		if (fileExists(pathToFile)){
			coloredLog("Index found: ", pathToFile, GREEN);
			return &(*it);
		}
	}
	coloredLog("No index found: ", _path, RED);
	return NULL;
}

void HttpResponse::GenerateErrorBody() {
	coloredLog("Error page generated: ", "", RED);
	coloredLog("Error code: ", toString(_statusCode), RED);
	coloredLog("Error message: ", _statusMessage, RED);
	this->setBody("    <style>\n"
				  "        body {\n"
				  "            background-color: #111;\n"
				  "            color: #fff;\n"
				  "            font-family: Arial, sans-serif;\n"
				  "            display: flex;\n"
				  "            justify-content: center;\n"
				  "            align-items: center;\n"
				  "            height: 100vh;\n"
				  "            margin: 0;\n"
				  "        }\n"
				  "        </style>\n");
	this->appendBody("<html><body><h1>\n" + toString(_statusCode) + " " + _statusMessage + "\n</h1></body></html>");

}

void HttpResponse::getFileFromPostAndSaveIt(HttpRequest request) {
	coloredLog("Building POST response: ", "", BLUE);

	std::string *boundary = request.getHeader("Content-Type");
	std::string filename = "file";
	std::string fileContent;
	if (boundary == NULL || boundary->find("boundary=") == std::string::npos)
		fileContent = request.getBody();
	else{
		try {
			ExtractImgInsideBoudaries(request, boundary, filename, fileContent);
		}
		catch (std::exception &e) {
			coloredLog("Error: ", e.what(), RED);
			this->buildErrorPage(500);
		}
	}
	createFile(filename, fileContent);
}

void HttpResponse::ExtractImgInsideBoudaries(const HttpRequest &request,
											 std::string *boundary,
											 std::string &filename,
											 std::string &fileContent) const {
	coloredLog("Boundary found: ", *boundary, BLUE);
	*boundary = boundary->substr(boundary->find("boundary=") + 9);
	std::string body = request.getBody();

	filename = body.substr(body.find("filename=") + 9);
	char delimiter = filename[0];
	filename = filename.substr(1, filename.substr(1).find(delimiter));
	coloredLog("File name: ", filename, BLUE);

	fileContent = body.substr(body.find(*boundary) + boundary->length() + 2);
	//trim all headers
	fileContent = fileContent.substr(fileContent.find("\r\n\r\n") + 4);
	fileContent = fileContent.substr(0, fileContent.find(*boundary) - 2);
	if (fileContent.size() >= 2 && fileContent[fileContent.size() - 2] == '\r' && fileContent[fileContent.size() - 1] == '\n')
		fileContent.erase(fileContent.size() - 2, 2);
}
