#include "config/ConfigContext.hpp"


Context::Context() {}

Context::Context(const std::string& name, const std::string& arguments, const std::string& content): _name(name), _arguments(arguments), _content(content) {}

void Context::addDirective(const std::string &name, const std::string &content) {
	_directives.insert(std::make_pair(name, content));
}

void Context::addContext(const Context context) {
	_subContexts.push_back(context);
}

const std::vector<Context>& Context::getContexts() const {
	return _subContexts;
}

std::string Context::getName() const {
	return _name;
}

void Context::inheritDirectives() {
	std::vector<Context>::iterator	it;
	for (it = _subContexts.begin(); it != _subContexts.end(); it++) {
		it->_directives = _directives;
	}
}

std::string	Context::getContent() const {
	return _content;
}