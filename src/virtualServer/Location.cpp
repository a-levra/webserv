#include "virtualServer/Location.hpp"
#include "utils/utils.hpp"
#include "logger/logging.hpp"

#include <cstdlib>

Location::Location(): _URI(""), _root("html"),
					  _clientMaxBodySize(1),
					  _autoIndex(false),
					  _hasReturn(false) {
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
											_autoIndex(false),
											_hasReturn(false) {
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
	_URI = other._URI;
	_root = other._root;
	_index = other._index;
	_allowMethods = other._allowMethods;
	_errorPage = other._errorPage;
	_return = other._return;
	_clientMaxBodySize = other._clientMaxBodySize;
	_autoIndex = other._autoIndex;
	_cgiPath = other._cgiPath;
	_hasReturn = other._hasReturn;
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
	else if (name == "cgi_path")
		addCGIPathDirective(content);
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

void Location::addCGIPathDirective(const std::string &content) {
	std::vector<std::string>	arguments = splitWhiteSpace(content);

	std::vector<std::string>::const_iterator it;
	for (it = arguments.begin(); it != arguments.end(); it++) {
		std::vector<std::string> extensionPath = splitDelimiter(*it, ':');
		_cgiPath.insert(std::make_pair(extensionPath[0], extensionPath[1]));
	}
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

const std::map<std::string, std::string> &Location::getCGIPath() const {
	return _cgiPath;
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
	_hasReturn = true;
}

void Location::display() {
//	logging::debug(B_PURPLE  "Location : " THIN );
	logging::debug(COLOR_RESET BOLD  "\tURI : " THIN + _URI + COLOR_RESET);
	logging::debug(COLOR_RESET BOLD  "\troot : " THIN + _root);
//	logging::debug(B_PURPLE  "\tclient_max_body_size : " THIN + toString(_clientMaxBodySize));
//	logging::debug(B_PURPLE  "\tautoindex : " THIN + toString(_autoIndex));
//	logging::debug(B_PURPLE  "\tindex : " THIN );
//	for (size_t i = 0; i < _index.size(); i++) {
//		logging::debug("\t\t" THIN + _index[i]);
//	}
//	logging::debug(B_PURPLE "allow_methods : " THIN );
//	for (size_t i = 0; i < _allowMethods.size(); i++) {
//		logging::debug("\t\t" THIN + _allowMethods[i]);
//	}
//	logging::debug(B_PURPLE  "error_page : " THIN );
//	for (size_t i = 0; i < _errorPage.first.size(); i++) {
//		logging::debug("\t\t" THIN + toString(_errorPage.first[i]));
//	}
//	logging::debug("\t\t" THIN + _errorPage.second);
//	logging::debug(B_PURPLE  "\treturn : " THIN );
//	logging::debug("\t\t" THIN + toString(_return.first));
//	logging::debug("\t\t" THIN + _return.second);
}

bool	Location::isAllowedMethod(const std::string& method) const{
	std::vector<std::string>::const_iterator it;
	for (it = _allowMethods.begin(); it != _allowMethods.end(); it++) {
		if (*it == method)
			return true;
	}
	return false;
}

bool Location::hasCGI() const {
	return _cgiPath.size() > 0;
}

bool Location::hasReturn() const {
	return _hasReturn;
}
