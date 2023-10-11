#include "config/ConfigContext.hpp"


Context::Context() {}

Context::Context(const std::string& name, const std::string& arguments, const std::string& content): _name(name), _arguments(arguments), _content(content) {}

void Context::addDirective(const std::string &name, const std::string &content) {
	_directives[name] = content;
}

void Context::addContext(const Context context) {
	_subContexts.push_back(context);
}

void Context::inheritDirectives() {
	std::vector<Context>::iterator	it;
	for (it = _subContexts.begin(); it != _subContexts.end(); it++) {
		it->_directives = _directives;
	}
}

std::string Context::getName() const {
	return _name;
}

std::string	Context::getContent() const {
	return _content;
}

std::string Context::getArguments() const {
	return _arguments;
}

void Context::setName(std::string name) {
	_name = name;
}

std::vector<Context>& Context::getSubContexts() {
	return _subContexts;
}

const std::vector<Context> &Context::getConstSubContexts() const {
	return _subContexts;
}

const std::map<std::string, std::string>&	Context::getDirectives() const {
	return _directives;
}
