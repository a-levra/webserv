#include "config/ConfigParser.hpp"
#include "config/ConfigLexer.hpp"

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
	ConfigLexer	lexer(fileName);
	if (lexer.getCodeError() != ConfigLexer::NO_ERROR) {
		std::cout << lexer.getError() << std::endl;
	}
	return 0;
}