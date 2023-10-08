#include "server/parser/ConfigParser.hpp"

#include <fstream>
#include <iostream>

ConfigParser::ConfigParser(void)
{

}

ConfigParser::ConfigParser(const ConfigParser &other)
{
	*this = other;
}

ConfigParser::~ConfigParser(void)
{

}

ConfigParser &ConfigParser::operator=(const ConfigParser &other)
{
	if (this == &other)
		return (*this);
	return (*this);
}

int ConfigParser::parseConfigFile(const std::string &fileName) {
	std::ifstream	file(fileName.c_str());
	std::string		line;
	std::string		fileContent;

	if (!file.is_open())
		return (-1);
	while (getline(file, line)) {
		size_t	commentSeparator = line.find('#');
		if (commentSeparator != std::string::npos)
			line.erase(commentSeparator);
		fileContent += line;
	}
//	std::cout << fileContent << std::endl;
	_parseContent(fileContent);
	return 0;
}

int ConfigParser::_parseContent(const std::string &content) {
	if (content.size() == 0)
		return 0;
	if (_isDirectiveOrContext(content)) {
		std::string directiveName = content.substr(0, content.find(';'));
		if (directiveName.size() == 0) {
			std::cout << "Invalid directive" << std::endl;
			return -1;
		}
		std::cout << "Directive: " << directiveName << std::endl;
		_parseContent(content.substr(content.find(';') + 1));
	}
	else {
		std::string contextName = content.substr(0, content.find('{'));
		ssize_t isValidContext = _parseContext(content.substr(content.find('{')));
		std::string contextContent;
		if (isValidContext != -1)
			contextContent = content.substr(contextName.size() + 1, isValidContext - 1);
		else
			contextContent = "";
		std::cout << "Context: " << contextName << " Validity: " << (isValidContext != -1)
		<< " Content: " << contextContent << std::endl;
		if (isValidContext != -1)
		{
			_parseContent(contextContent);
//			std::cout << content.substr(content.find('{')).substr(isValidContext + 1) << std::endl;
			_parseContent(content.substr(content.find('{')).substr(isValidContext + 1));
		}
	}
	return 0;
}

ssize_t ConfigParser::_parseContext(const std::string &content) {
	size_t i = 0;
	size_t openBrackets = 0;

//	std::cout << content << std::endl;
	if (content[0] == '{') {
		openBrackets++;
		i++;
	}
	for (; i < content.size(); i++) {
		if (content[i] == '{')
			openBrackets++;
		else if (content[i] == '}' && openBrackets == 0)
			return -1;
		else if (content[i] == '}')
			openBrackets--;
		if (openBrackets == 0)
			return i;
	}
	if (openBrackets != 0)
		return -1;
	return i;
}

int ConfigParser::_isDirectiveOrContext(const std::string &content) {
	size_t directiveIndex = content.find(';');
	size_t contextIndex = content.find('{');
	if (directiveIndex < contextIndex)
		return 1;
	return 0;
}