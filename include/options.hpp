#ifndef OPTIONS_HPP
# define OPTIONS_HPP

#include "logger/Logger.hpp"

#define CONFIG_FILE_EXTENSION ".conf"

struct Options {
	std::string			logFile;
	std::string 		configFile;
	enum Logger::level	logLevel;
	bool				logColor;
	bool				help;
	bool				syntax;
};

bool getOptions(int argc, char **argv, Options &options);

#endif