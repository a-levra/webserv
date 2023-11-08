#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#include <fstream>
#include <cstdlib>
#include <cstring>

#include "HttpMessages/HttpResponse.hpp"
#include "utils/utils.hpp"
#include "logger/logging.hpp"

HttpResponse::HttpResponse(class HttpRequest &r)
	: _location(NULL), _request(r) {}

HttpResponse::HttpResponse(const HttpResponse &other)
	: AHttpMessage(other), _request(other._request) { *this = other; }

HttpResponse::~HttpResponse(void) {}

HttpResponse &HttpResponse::operator=(const HttpResponse &other) {
	if (this != &other) {
		AHttpMessage::operator=(other);
		_request = other._request;
		_location = other._location;
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
std::string HttpResponse::getResponse(Server &server, const Client &client) {

	if (_request.isInvalid())
		return buildErrorPage(BAD_REQUEST);

	const std::string *host = _request.getHeader("Host");
	if (host == NULL)
		return buildErrorPage(BAD_REQUEST);

	VirtualServer *vs = server.getVirtualServer(client.getEntryIPAddress(),
												client.getEntryPort(), *host);
	if (vs == NULL)
		return buildErrorPage(NOT_FOUND);

	_requestUri = _request.getRequestUri();
	logging::debug(B_PURPLE  "URI requested: " + _requestUri + COLOR_RESET);
	_location = vs->getLocation(_requestUri);
	if (_location == NULL)
		return buildErrorPage(NOT_FOUND);
	logging::debug(B_GREEN  "Location best match: " THIN COLOR_RESET);
	_location->display();
	build();
	return _rawMessage;
}

void HttpResponse::build() {

	if (!_location->isAllowedMethod(_request.getMethod())) {
		buildErrorPage(METHOD_NOT_ALLOWED);
		return;
	}
    if (!checkRequestMaxBodySize()) {
        buildErrorPage(PAYLOAD_TOO_LARGE);
        return;
    }
    if (_request.getValidity() == HttpRequest::VALID_AND_INCOMPLETE_REQUEST) {
        logging::debug(B_YELLOW "Request is incomplete" COLOR_RESET);
        _rawMessage = "";
        return;
    }
    logging::debug(B_BLUE "Building " THIN + _request.getMethod() + " " + _location->getURI());
	if (_request.getMethod() == "GET")
		buildGet();
	else if (_request.getMethod() == "POST")
		buildPost();
	else if (_request.getMethod() == "DELETE")
		buildDelete();
	else
		buildErrorPage(NOT_IMPLEMENTED);
}

bool HttpResponse::checkRequestMaxBodySize() {
	if ((float) _request.getBody().size() / 1000000
		> (float) _location->getClientMaxBodySize()) {
		logging::debug("Payload to large : ");
		logging::debug(
			"\tBody size : " + toString(_request.getBody().size() / 1000000) + "m");
		logging::debug(
			"Max body size : " + toString(_location->getClientMaxBodySize()) + "m");
		_request.addError(HttpRequest::PAYLOAD_TOO_LARGE_ERROR);
		return false;
	}
	return true;
}

void HttpResponse::buildGet() {
	std::string response;

	if (_location->hasReturn()) {
		_statusCode = _location->getReturn().first;
        _statusMessage = "OK";
		setHeader("Location", _location->getReturn().second);
		buildRawMessage();
		return;
	}
	if (_requestUri[_requestUri.size() - 1] == '/'
		&& _location->getAutoIndex()) {
		if (!buildListingDirectory()) {
			buildErrorPage(INTERNAL_SERVER_ERROR);
			return;
		}
        _statusCode = OK;
        _statusMessage = "OK";
        buildRawMessage();
		return;
	}
	if (_location->hasCGI()) {
		buildCGIGet();
		return;
	}
	generateBody();
	buildRawMessage();
}

void HttpResponse::generateBody() {
	const std::string *file;

	std::string res = getResource();
	if (!res.empty()) {
		file = tryGetFile(res);
		if (file == NULL) {
			buildErrorPage(NOT_FOUND);
			return;
		}
	} else
		file = getFirstValidIndex();

	if (file == NULL) {
		logging::debug("No index valid: " + _requestUri);
		buildErrorPage(INTERNAL_SERVER_ERROR);
		return;
	}
	bool success;
	_body =
		readFileToString(_location->getRoot() + _location->getURI() + "/" + *file,
						 success);
	if (!success) {
		buildErrorPage(INTERNAL_SERVER_ERROR);
		return;
	} else {
		setStatusCode(OK);
		setStatusMessage("OK");
		setBody(_body);
		return;
	}
}

void HttpResponse::buildPost() {
	if (_location->hasCGI()) {
		buildCGIPost();
		return;
	}
	if (!getFileFromPostAndSaveIt()) {
		buildRawMessage();
		return;
	}
	_statusCode = OK;
	_statusMessage = "OK";
	buildRawMessage();
}

void HttpResponse::buildDelete() {
	logging::debug("Building DELETE response: ");
	_request.displayRequest();
	std::string res = getResource();
	logging::debug("Resource: " + res);
	std::string file = _location->getRoot() + "/" + _requestUri;
	if (!fileExists(file)) {
		buildErrorPage(NOT_FOUND);
		return;
	} else if (std::remove(file.c_str()) != 0) {
		buildErrorPage(INTERNAL_SERVER_ERROR);
		return;
	}
	_statusCode = OK;
	_statusMessage = "OK";
	buildRawMessage();
}

std::string &HttpResponse::buildErrorPage(int errorCode) {
	std::string response;
	setStatusCode(errorCode);
	std::string error = toString(errorCode);
	std::string errorName;
	switch (errorCode) {
		case BAD_REQUEST: errorName = "Bad Request";
			break;
		case NOT_FOUND: errorName = "Not Found";
			break;
		case METHOD_NOT_ALLOWED: errorName = "Method Not Allowed";
			break;
		case PAYLOAD_TOO_LARGE: errorName = "Payload Too Large";
			break;
		case INTERNAL_SERVER_ERROR: errorName = "Internal Server Error";
			break;
		case NOT_IMPLEMENTED: errorName = "Not Implemented";
			break;
		case HTTP_VERSION_NOT_SUPPORTED: errorName = "HTTP Version Not Supported";
			break;
		default: errorName = "Unknown Error";
			break;
	}
	setStatusMessage(errorName);
	setHeader("Content-type", "text/html");
	GenerateErrorBody();
	buildRawMessage();
	return _rawMessage;
}

void HttpResponse::setHeader(const std::string &header,
							 const std::string &content) {
	_headers[header] = content;
}

const std::string *HttpResponse::getFirstValidIndex() const {
	if (!_location)
		return NULL;
	const std::vector<std::string> &index = _location->getIndex();
	std::vector<std::string>::const_iterator it;
	logging::debug(B_BLUE "Searching a valid index.. " THIN);
	for (it = index.begin(); it != index.end(); it++) {
		std::string
			pathToFile = _location->getRoot() + _location->getURI() + "/" + *it;
		if (fileExists(pathToFile)) {
			logging::debug(B_GREEN "Index found : " THIN + pathToFile);
			return &(*it);
		}
	}
	logging::debug(B_RED "No index found: " THIN + _location->getURI());
	return NULL;
}

void HttpResponse::GenerateErrorBody() {
	std::string specificErrorPage = getSpecificErrorPage(_statusCode);
	bool success;
	if (!specificErrorPage.empty()) {
		setBody(readFileToString(
			_location->getRoot() + _location->getURI() + "/" + specificErrorPage,
			success));
		if (!success) {
			logging::debug(B_RED "Failed to open specific error page !" THIN);
			if (_statusCode != INTERNAL_SERVER_ERROR) {
				buildErrorPage(INTERNAL_SERVER_ERROR);
				return;
			} else {
				setStatusCode(INTERNAL_SERVER_ERROR);
				setStatusMessage("Internal server error");
			}
		} else
			return;
	}

	logging::debug("Error page generated: ");
	logging::debug("Error code: " + toString(_statusCode));
	logging::debug("Error message: " + _statusMessage);
	std::string additionnalInfo;
	if (_statusCode == PAYLOAD_TOO_LARGE) {
		additionnalInfo =
			"The maximum size of the payload for \"" + _location->getURI()
				+ "\" is "
				+ toString(this->_location->getClientMaxBodySize()) + " Mb.";
		additionnalInfo += "</h3><h3>Current payload size: "
			+ toString(_request.getBody().size() / 1000000) + " Mb.";
		if (_request.getHeader(CONTENT_LENGTH)) {
			additionnalInfo += "</h3><h3>Total payload you tried to send: ";
			std::string payloadSizeStr = *(_request.getHeader(CONTENT_LENGTH));
			size_t payloadSize = std::strtod(payloadSizeStr.c_str(), 0);
			additionnalInfo += toString(payloadSize / 1000000) + " Mb.";
		}
	} else
		additionnalInfo = _request.getErrors();
	appendBody("<html>"
			   "<body>"
			   "<h1>\n" + toString(_statusCode) + " " + _statusMessage + "\n</h1>" \
                             "<h2>\n" + _request.getMethod() + " "
				   + _request.getRequestUri() + "\n</h2>" \
                             "<h3>\n" + additionnalInfo + "\n</h3>" \
                    "</body></html>");

}

bool HttpResponse::getFileFromPostAndSaveIt() {
	logging::debug("Building POST response ");

	std::string *boundary = _request.getHeader("Content-Type");
	std::string fullPath;
	std::string fileContent;
	if (boundary == NULL || boundary->find("boundary=") == std::string::npos) {
		if (_requestUri[_requestUri.size() - 1] == '/') {
			fullPath = _location->getRoot() + "/" + _requestUri + "/file";
		} else {
			fullPath = _location->getRoot() + "/" + _requestUri;
		}
		fileContent = _request.getBody();
	}
	else {
		try {
			std::string fileName;
			ExtractImgInsideBoundaries(boundary, fileName, fileContent);
			fullPath = _location->getRoot() + "/" + _requestUri + "/" + fileName;
		}
		catch (std::exception &e) {
			this->buildErrorPage(INTERNAL_SERVER_ERROR);
			return false;
		}
	}
	if (!createFile(fullPath, fileContent)) {
		buildErrorPage(INTERNAL_SERVER_ERROR);
		return false;
	}
	return true;
}

void HttpResponse::ExtractImgInsideBoundaries(std::string *boundary,
											  std::string &filename,
											  std::string &fileContent) const {
	*boundary = boundary->substr(boundary->find("boundary=") + 9);
	std::string body = _request.getBody();

	filename = body.substr(body.find("filename=") + 9);
	char delimiter = filename[0];
	filename = filename.substr(1, filename.substr(1).find(delimiter));

	fileContent = body.substr(body.find(*boundary) + boundary->length() + 2);
	//trim all headers
	fileContent = fileContent.substr(fileContent.find(CRLF CRLF) + 4);
	fileContent = fileContent.substr(0, fileContent.find(*boundary) - 2);
	if (fileContent.size() >= 2 && fileContent[fileContent.size() - 2] == '\r'
		&& fileContent[fileContent.size() - 1] == '\n')
		fileContent.erase(fileContent.size() - 2, 2);
}

std::string HttpResponse::getResource() const {
	std::string resource;
	if (_location)
		resource = _requestUri.substr(_location->getURI().length());
	if (!resource.empty())
		logging::debug(B_PURPLE "Resource detected: " THIN + resource);
	return resource;
}

const std::string *HttpResponse::tryGetFile(const std::string &resource) {
	if (!_location)
		return NULL;
	std::string
		pathToFile = _location->getRoot() + _location->getURI() + "/" + resource;
	logging::debug("path to resource: " + pathToFile);
	if (fileExists(pathToFile))
		return &resource;
	logging::debug(B_RED "File not found: " THIN + pathToFile);
	return NULL;
}

std::string HttpResponse::getSpecificErrorPage(int code) {
	if (!_location)
		return "";
	std::pair<std::vector<int>, std::string> errorPage;
	logging::debug(B_BLUE "Checking if a specific error page exist " COLOR_RESET);
	logging::debug(B_BLUE "\tfor code : " THIN + toString(code) + COLOR_RESET);
	logging::debug(
		B_BLUE "\tand location : " THIN + _location->getURI() + COLOR_RESET);
	errorPage = _location->getErrorPage();

	std::vector<int>::iterator it;
	for (it = errorPage.first.begin(); it != errorPage.first.end(); it++) {
		if (*it == code) {
			logging::debug(
				B_GREEN "Specific error page found: " THIN + errorPage.second
					+ COLOR_RESET);
			return errorPage.second;
		}
	}
	logging::debug(B_BLUE "No specific error page found " COLOR_RESET);
	return "";
}

void HttpResponse::buildRawMessage() {
	setHeader("Date", getDate());
	setHeader("Server", "webserv");
	setHeader("Content-Length", toString(_body.length()));
	std::string response = HTTP_VERSION  " " + toString(this->_statusCode)
		+ " " + _statusMessage + CRLF;
	std::map<std::string, std::string>::iterator it;
	for (it = _headers.begin(); it != _headers.end(); it++) {
		response += it->first + ": " + it->second + CRLF;
	}
	response += CRLF;
	response += _body;
	_rawMessage = response;
}
HttpRequest::REQUEST_VALIDITY HttpResponse::getRequestValidity() const {
	return _request.getValidity();
}

bool HttpResponse::buildListingDirectory() {
	std::string directoryPath;
	std::stringstream directoryListing;
	DIR *dir;
	dirent *dirEntry;

	directoryPath = _location->getRoot() + '/' + _requestUri;
	dir = opendir(directoryPath.c_str());
	directoryListing << "<html>" << std::endl
					 << "\t<head>" << std::endl
					 << "\t\t<title>Index of " << _request.getRequestUri()
					 << "</title>" << std::endl
					 << "\t</head>" << std::endl
					 << "\t<body>" << std::endl
					 << "\t\t<h1>Index of " << _request.getRequestUri() << "</h1>"
					 << std::endl
					 << "\t\t<hr>" << std::endl
					 << "\t\t<pre>" << std::endl;
	if (dir == NULL) {
		return false;
	}
	dirEntry = readdir(dir);
	while (dirEntry) {
		if (isDirectory(directoryPath + dirEntry->d_name))
			directoryListing << "\t\t\t<a href=\"" << dirEntry->d_name << "/\">"
							 << dirEntry->d_name << "/</a>" << std::endl;
		else
			directoryListing << "\t\t\t<a href=\"" << dirEntry->d_name << "\">"
							 << dirEntry->d_name << "</a>" << std::endl;
		dirEntry = readdir(dir);
	}
	closedir(dir);
	directoryListing << "\t\t</pre>" << std::endl
					 << "\t\t<hr>" << std::endl
					 << "\t</body>" << std::endl
					 << "</html>" << std::endl;
	_body = directoryListing.str();
	setHeader("Content-Type", "text/html");
	return true;
}