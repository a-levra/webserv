#ifndef CONFIGLEXER_HPP
#define CONFIGLEXER_HPP

#include "config/ConfigContext.hpp"
#include <string>
#include <vector>
#include <map>

class ConfigLexer
{
public:
	enum tokenType
	{
		UNDEFINED,
		DIRECTIVE,
		CONTEXT,
	};

	enum codeError
	{
		NO_ERROR,
		INVALID_FILE,
		EMPTY_DIRECTIVE,
		UNCLOSE_DIRECTIVE,
		UNCLOSE_CONTEXT,
		CLOSE_UNOPENED_CONTEXT,
		EMPTY_NAME_CONTEXT,
	};

	ConfigLexer();
	ConfigLexer(const std::string& fileName);
	ConfigLexer(const ConfigLexer& other);
	~ConfigLexer();

	ConfigLexer& operator=(const ConfigLexer& other);

	ConfigLexer::codeError	getCodeError() const;
	std::string				getError() const;
	Context&				getMainContext();
	const Context&			getConstMainContext() const ;

private:

	bool					_analyzeContent(Context& currentContext,
											std::string content);
	std::string::size_type	_analyzeDirective(Context& currentContext,
											  const std::string& content);
	std::string::size_type	_analyzeContext(Context& currentContext,
											const std::string& content);
	bool					_analyzeSubContexts(Context& currentContext);

	static tokenType	_identifyNextToken(const std::string& content);
	static std::pair<std::string::size_type, codeError> _getEndContext(const std::string& content);

	codeError	_error;
	std::string _fileContent;
	Context		_mainContext;

};

#endif
