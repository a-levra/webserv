#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include <string>

class ConfigParser
{
public:
	ConfigParser(void);
	ConfigParser(const ConfigParser &other);
	~ConfigParser(void);

	ConfigParser &operator=(const ConfigParser &other);

	int	parseConfigFile(const std::string& fileName);


private:
	int	_isDirectiveOrContext(const std::string& content);
	int	_parseContent(const std::string& content);
	ssize_t _parseContext(const std::string& content);

};

typedef struct s_context {
	std::vector<struct s_context>	contexts;
	std::vector<std::string> directives;
}	t_context;

#endif
