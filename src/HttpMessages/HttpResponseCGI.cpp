#include "HttpMessages/HttpResponse.hpp"

#include "utils/utils.hpp"
#include "cgi/CGI.hpp"
#include "logger/logging.hpp"

void HttpResponse::buildCGIGet() {
	if (!searchCGIPath()) {
		buildErrorPage(NOT_FOUND);
		return;
	}
	CGI cgi;
	CGI::codeError code = cgi.execute(*this);
	if (code != CGI::SUCCESSFUL) {
		if (code == CGI::TIMEOUT) {
			logging::warning(_cgiFile + " timeout after "
							 + toString(CGI_TIMEOUT_MS) + "ms");
		}
		if (code == CGI::FAILED) {
			logging::warning(_cgiFile + " failed. StdErr: " + cgi.getStdErr());
		}
		_body = cgi.getStdErr();
		buildErrorPage(INTERNAL_SERVER_ERROR);
		return;
	}
	parseCGIOutput(cgi.getStdOut());
	_statusCode = 200;
	_statusMessage = "OK";
	buildRawMessage();
	displayRequest();
}

bool HttpResponse::findCGIExtension(const std::string &path) {
	std::map<std::string, std::string>::const_iterator it;
	const std::map<std::string, std::string>& extensions = _location->getCGIPath();
	std::string::size_type extensionPos = std::string::npos;
	for (it = extensions.begin(); it != extensions.end(); it++) {
		if (path.length() > it->first.length()
			&& path.substr(path.length() - it->first.length()) == it->first) {
			extensionPos = path.length() - it->first.length();
			_cgiExtension = std::make_pair(it->first, it->second);
		}
	}
	for (it = extensions.begin(); it != extensions.end(); it++) {
		std::string::size_type pos = path.find(it->first + "/");
		if (pos != std::string::npos && pos < extensionPos) {
			extensionPos = pos;
			_cgiExtension = std::make_pair(it->first, it->second);
		}
	}
	if (extensionPos == std::string::npos)
		return false;
	extractCGIPath(path, extensionPos);
	return true;
}

void HttpResponse::extractCGIPath(const std::string &path,
								  std::string::size_type extensionPos) {
	std::string::size_type rpos = path.rfind('/', extensionPos);
	std::string::size_type lpos = path.find('/', extensionPos);
	if (rpos == std::string::npos)
		_cgiPath = "";
	else
		_cgiPath = path.substr(0, rpos);
	if (lpos == std::string::npos)
		_cgiPathInfo = "";
	else
		_cgiPathInfo = path.substr(lpos);
	if (rpos == std::string::npos)
		_cgiFile = path.substr(0, lpos);
	else if (lpos == std::string::npos)
		_cgiFile = path.substr(rpos);
	else
		_cgiFile = path.substr(rpos, lpos - rpos);
	_cgiFile = trimCharset(_cgiFile, "/");
}


bool HttpResponse::searchCGIPath() {
	std::string path = _location->getRoot() + "/" +  _requestUri;
	if (!findCGIExtension(path)) {
		const std::string* index = getFirstValidIndex();
		if (index == NULL) {
			return false;
		}
		path += "/" + *index;
		if (!findCGIExtension(path)) {
			return false;
		}
	}
	if (!fileExists(_cgiPath + "/" + _cgiFile)) {
		return false;
	}
	return true;
}

void HttpResponse::parseCGIOutput(const std::string& cgiOutput) {
	size_t headerEnd = cgiOutput.find(CRLF CRLF);
	if (headerEnd == std::string::npos) {
		_body = cgiOutput;
	} else {
		std::string headerString = cgiOutput.substr(0, headerEnd);
		_body = cgiOutput.substr(headerEnd + CRLF_SIZE * 2);
		size_t prevPos = 0;
		while (prevPos < headerString.size()) {
			size_t pos = headerString.find(CRLF, prevPos);
			if (pos == std::string::npos) {
				pos = headerString.size();
			}
			std::string headerLine = headerString.substr(prevPos, pos - prevPos);
			prevPos = pos + CRLF_SIZE;
			size_t colonPos = headerLine.find(": ");
			if (colonPos != std::string::npos) {
				std::string headerName = headerLine.substr(0, colonPos);
				std::string headerValue = headerLine.substr(colonPos + 2);
				setHeader(headerName, headerValue);
			}
		}
	}
}

void HttpResponse::buildCGIPost() {
	if (!searchCGIPath()) {
		buildErrorPage(NOT_FOUND);
		return;
	}
	CGI cgi;
	CGI::codeError code = cgi.execute(*this);
	if (code != CGI::SUCCESSFUL) {
		if (code == CGI::TIMEOUT) {
			logging::warning(_cgiFile + " timeout after "
			+ toString(CGI_TIMEOUT_MS) + "ms");
		}
		if (code == CGI::FAILED) {
			logging::warning(_cgiFile + " failed. StdErr: " + cgi.getStdErr());
		}
		_body = cgi.getStdErr();
		buildErrorPage(INTERNAL_SERVER_ERROR);
		return;
	}
	parseCGIOutput(cgi.getStdOut());
	_statusCode = 200;
	_statusMessage = "OK";
	buildRawMessage();
	displayRequest();
}


const std::string &HttpResponse::getCGIPath() const {
	return _cgiPath;
}

const std::string &HttpResponse::getCGIFile() const {
	return _cgiFile;
}

const std::string &HttpResponse::getCGIPathInfo() const {
	return _cgiPathInfo;
}

const std::pair<std::string, std::string> &HttpResponse::getCGIExtension() const {
	return _cgiExtension;
}

HttpRequest &HttpResponse::getRequest() const {
	return _request;
}