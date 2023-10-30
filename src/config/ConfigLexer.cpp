#include "config/ConfigLexer.hpp"
#include "utils/utils.hpp"

#include <fstream>
#include <algorithm>
#include <cctype>

ConfigLexer::ConfigLexer() {}

ConfigLexer::ConfigLexer(const std::string &fileName) {
	std::ifstream	file(fileName.c_str());
	std::string		line;
	std::string		fileContent;

	_error = NO_ERROR;
	if (!file.is_open()) {
		_error = INVALID_FILE;
		return;
	}
	while (getline(file, line)) {
		size_t	commentSeparator = line.find('#');
		if (commentSeparator != std::string::npos)
			line.erase(commentSeparator);
		fileContent += line;
	}
	_mainContext = Context("main", "", _fileContent);
	_fileContent = fileContent;
	_analyzeContent(_mainContext, _fileContent);
}

ConfigLexer::ConfigLexer(const ConfigLexer &other) {
	*this = other;
}

ConfigLexer::~ConfigLexer() {}

ConfigLexer &ConfigLexer::operator=(const ConfigLexer &other) {
	if (this == &other)
		return *this;
	_error = other._error;
	_fileContent = other._fileContent;
	_mainContext = other._mainContext;
	return *this;
}

ConfigLexer::codeError	ConfigLexer::getCodeError() const {
	return _error;
}

std::string ConfigLexer::getError() const {
	switch (_error) {
		case NO_ERROR:
			return "success";
		case INVALID_FILE:
			return "unable to open file";
		case EMPTY_DIRECTIVE:
			return "empty directive found";
		case UNCLOSE_DIRECTIVE:
			return "unclose directive found";
		case UNCLOSE_CONTEXT:
			return "unclose context found";
		case CLOSE_UNOPENED_CONTEXT:
			return "closing unopened context";
		case EMPTY_NAME_CONTEXT:
			return "empty name context";
	}
	return "";
}

Context &ConfigLexer::getMainContext() {
	return  _mainContext;
}

const Context &ConfigLexer::getConstMainContext() const {
	return _mainContext;
}

bool ConfigLexer::_analyzeContent(Context& currentContext, std::string content) {
	tokenType type = _identifyNextToken(content);
	std::string::size_type	pos;
	while (type != UNDEFINED) {
		if (type == DIRECTIVE)
			pos = _analyzeDirective(currentContext, content);
		else if (type == CONTEXT)
			pos = _analyzeContext(currentContext, content);
		if (_error != NO_ERROR)
			return false;
		content = content.substr(pos + 1);
		type = _identifyNextToken(content);
	}
	trim(content);
	if (!content.empty())
		_error = UNCLOSE_DIRECTIVE;
	if (_error != NO_ERROR)
		return false;
	_analyzeSubContexts(currentContext);
	return true;
}

bool ConfigLexer::_analyzeSubContexts(Context& currentContext) {
	std::vector<Context>&	subContexts = currentContext.getSubContexts();
	std::vector<Context>::iterator it;
	for (it = subContexts.begin(); it != subContexts.end(); it++) {
		if (_error != NO_ERROR)
			return false;
		_analyzeContent(*it, it->getContent());
	}
	return true;
}

std::string::size_type ConfigLexer::_analyzeDirective(Context& currentContext,
													  const std::string& content)
{
	std::string::size_type pos = content.find(';');
	std::string directive = content.substr(0, pos);
	std::string::iterator it;

	directive = trim(directive);
	it = std::find_if(directive.begin(), directive.end(), isspace);
	std::string directiveName(directive.begin(), it);
	it = std::find_if(it, directive.end(), isspace);
	std::string directiveContent(it, directive.end());
	directiveContent = trim(directiveContent);
	if (directiveName.empty()) {
		_error = EMPTY_DIRECTIVE;
		return pos;
	}
	currentContext.addDirective(directiveName, directiveContent);
	return pos;
}

std::string::size_type	ConfigLexer::_analyzeContext(Context& currentContext,
													 const std::string& content)
{
	std::pair<std::string::size_type, codeError> pos = _getEndContext(content);
	if (pos.first == std::string::npos) {
		_error = pos.second;
		return pos.first;
	}
	std::string context = content.substr(0, pos.first + 1);
	std::string::iterator it;

	std::string header = context.substr(0, context.find('{'));
	header = trim(header);
	it = std::find_if(header.begin(), header.end(), isspace);
	std::string contextName(header.begin(), it);
	if (contextName.empty()) {
		_error = EMPTY_NAME_CONTEXT;
		return pos.first;
	}
	it = std::find_if(it, header.end(), std::not1(std::ptr_fun(isspace)));
	std::string contextArguments(it, header.end());
	std::string contextContent(std::find(context.begin(), context.end(), '{') + 1,
							   context.end() - 1);
	Context newContext(contextName, contextArguments, contextContent);
	currentContext.addContext(newContext);
	return pos.first;
}

std::pair<std::string::size_type, ConfigLexer::codeError>\
        ConfigLexer::_getEndContext(const std::string &content)
{
	std::string::size_type i = std::min(content.find('{'), content.find('}'));
	size_t openBrackets = 0;

	while (i < content.size()) {
		if (content[i] == '{')
			openBrackets++;
		else if (content[i] == '}' && openBrackets == 0)
			return std::make_pair(std::string::npos, CLOSE_UNOPENED_CONTEXT);
		else if (content[i] == '}')
			openBrackets--;
		if (openBrackets == 0)
			return std::make_pair(i, NO_ERROR);
		i++;
	}
	if (openBrackets != 0)
		return std::make_pair(std::string::npos, UNCLOSE_CONTEXT);
	return std::make_pair(i, NO_ERROR);
}

ConfigLexer::tokenType	ConfigLexer::_identifyNextToken(const std::string &content) {
	std::string::size_type	directiveIndex = content.find(';');
	std::string::size_type	openContextIndex = content.find('{');
	std::string::size_type	closeContextIndex = content.find('}');

	if (closeContextIndex < openContextIndex && closeContextIndex < directiveIndex)
		return CONTEXT;
	else if (directiveIndex > openContextIndex)
		return CONTEXT;
	else if (directiveIndex < openContextIndex)
		return DIRECTIVE;
	return UNDEFINED;
}