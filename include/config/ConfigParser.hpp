#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include <string>
#include <vector>
#include <cstdlib>
#include <map>

class ConfigParser
{
public:
	ConfigParser();
	ConfigParser(const ConfigParser &other);
	~ConfigParser();

	ConfigParser &operator=(const ConfigParser &other);

	int	parseConfigFile(const std::string& fileName);


private:

};



#endif
