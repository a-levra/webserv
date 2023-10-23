#include "virtualServer/Location.hpp"
#include "utils/utils.hpp"

#include <cstdlib>

Location::Location(): _URI(""), _root("html"),
					  _clientMaxBodySize(1),
					  _autoIndex(false) {
	std::vector<std::string> index;
	index.push_back("index.html");
	setIndex(index);
	std::vector<std::string> allowMethods;
	allowMethods.push_back("GET");
	allowMethods.push_back("POST");
	allowMethods.push_back("DELETE");
	setAllowMethods(allowMethods);

}

Location::Location(const std::string &URI): _URI(URI), _root("html"),
											_clientMaxBodySize(1),
											_autoIndex(false) {
	std::vector<std::string> index;
	index.push_back("index.html");
	setIndex(index);
	std::vector<std::string> allowMethods;
	allowMethods.push_back("GET");
	allowMethods.push_back("POST");
	allowMethods.push_back("DELETE");
	setAllowMethods(allowMethods);

}

Location::Location(const Location &other) { *this = other; }

Location::~Location(void) {}

Location &Location::operator=(const Location &other) {
	if (this == &other)
		return *this;
	this->_URI = other._URI;
	this->_root = other._root;
	this->_index = other._index;
	this->_allowMethods = other._allowMethods;
	this->_errorPage = other._errorPage;
	this->_return = other._return;
	this->_clientMaxBodySize = other._clientMaxBodySize;
	this->_autoIndex = other._autoIndex;
	return *this;
}

void Location::addDirective(const std::string &name, const std::string &content) {
	if (name == "root")
		setRoot(content);
	else if (name == "index")
		addIndexDirective(content);
	else if (name == "allow_methods")
		addAllowMethodsDirective(content);
	else if (name == "error_page")
		addErrorPageDirective(content);
	else if (name == "return")
		addReturnDirective(content);
	else if (name == "client_max_body_size")
		setClientMaxBodySize(std::atoi(content.c_str()));
	else if (name == "autoindex")
		setAutoIndex(content == "on");
}

void Location::addIndexDirective(const std::string &content) {
	setIndex(splitWhiteSpace(content));
}

void Location::addAllowMethodsDirective(const std::string &content) {
	setAllowMethods(splitWhiteSpace(content));
}

void Location::addErrorPageDirective(const std::string &content) {
	std::vector<std::string>	arguments = splitWhiteSpace(content);
	std::pair<std::vector<int>, std::string>	errorPage;
	std::vector<std::string>::iterator	it;

	errorPage.second = arguments[arguments.size() - 1];
	for (it = arguments.begin(); it != arguments.end() - 1; it++) {
		errorPage.first.push_back(std::atoi(it->c_str()));
	}
	setErrorPage(errorPage);
}

void Location::addReturnDirective(const std::string &content) {
	std::vector<std::string>	arguments = splitWhiteSpace(content);
	std::pair<int, std::string> result;

	result.first = std::atoi(arguments[0].c_str());
	result.second = arguments[1];
	setReturn(result);
}

std::string Location::getURI() const {
	return _URI;
}

std::string Location::getRoot() const {
	return _root;
}

size_t Location::getClientMaxBodySize() const {
	return _clientMaxBodySize;
}

bool Location::getAutoIndex() const {
	return _autoIndex;
}

const std::vector<std::string> &Location::getIndex() const {
	return _index;
}

const std::vector<std::string> &Location::getAllowMethods() const {
	return _allowMethods;
}

const std::pair<std::vector<int>, std::string> &Location::getErrorPage() const {
	return _errorPage;
}

const std::pair<int, std::string> &Location::getReturn() const {
	return _return;
}

void Location::setURI(const std::string &URI) {
	_URI = URI;
}

void Location::setRoot(const std::string &root) {
	_root = root;
}

void Location::setClientMaxBodySize(size_t clientMaxBodySize) {
	_clientMaxBodySize = clientMaxBodySize;
}

void Location::setAutoIndex(bool autoIndex) {
	_autoIndex = autoIndex;
}

void Location::setIndex(const std::vector<std::string>& index) {
	_index = index;
}

void Location::setAllowMethods(const std::vector<std::string>& allowMethods) {
	_allowMethods = allowMethods;
}

void Location::setErrorPage(const std::pair<std::vector<int>, std::string> &errorPage) {
	_errorPage = errorPage;
}

void Location::setReturn(const std::pair<int, std::string> &returnPage) {
	_return = returnPage;
}

void Location::display() {
	coloredLog("Location : ", "", PURPLE);
	coloredLog("URI : ", _URI, PURPLE);
	coloredLog("root : ", _root, PURPLE);
	coloredLog("client_max_body_size : ", toString(_clientMaxBodySize), PURPLE);
	coloredLog("autoindex : ", toString(_autoIndex), PURPLE);
	coloredLog("index : ", "", PURPLE);
	for (size_t i = 0; i < _index.size(); i++) {
		coloredLog("\t", _index[i], PURPLE);
	}
	coloredLog("allow_methods : ", "", PURPLE);
	for (size_t i = 0; i < _allowMethods.size(); i++) {
		coloredLog("\t", _allowMethods[i], PURPLE);
	}
	coloredLog("error_page : ", "", PURPLE);
	for (size_t i = 0; i < _errorPage.first.size(); i++) {
		coloredLog("\t", toString(_errorPage.first[i]), PURPLE);
	}
	coloredLog("\t", _errorPage.second, PURPLE);
	coloredLog("return : ", "", PURPLE);
	coloredLog("\t", toString(_return.first), PURPLE);
	coloredLog("\t", _return.second, PURPLE);
}

bool	Location::isAllowedMethod(const std::string& method) const{
	coloredLog("isAllowedMethod : ", method, PURPLE);

	std::vector<std::string>::const_iterator it;
	for (it = _allowMethods.begin(); it != _allowMethods.end(); it++) {
		if (*it == method)
			return true;
	}
	return true;
}
