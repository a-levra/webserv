#include "logger/Logger.hpp"

#include <ctime>
#include <iostream>
#include <fstream>

#include "utils/utils.hpp"

Logger::Logger() : _file(""), _hasFile(false),
				   _hasColor(false), _level(WARNING) { }

Logger::Logger(const Logger &other) {
	*this = other;
}

Logger::~Logger() { }

Logger &Logger::operator=(const Logger &other) {
	if (this == &other)
		return *this;
	_file = other._file;
	_hasFile = other._hasFile;
	_hasColor = other._hasColor;
	_level = other._level;
	return *this;
}

void Logger::log(enum Logger::level level, const std::string &message)  const {
	switch (level) {
		case DEBUG: debug(message);
			break;
		case INFO: info(message);
			break;
		case WARNING: warning(message);
			break;
		case ERROR: error(message);
			break;
		case CRITICAL: critical(message);
			break;
	}
}

void Logger::debug(const std::string &message) const {
	if (_level > DEBUG)
		return;
	std::string line = _getPrompt(DEBUG) + " " + message;
	if (_hasColor)
		std::cout << DEBUG_COLOR << line << RESET_COLOR << std::endl;
	else
		std::cout << line << std::endl;
	if (_hasFile)
		_appendInLogFile(line);
}

void Logger::info(const std::string &message) const {
	if (_level > INFO)
		return;
	std::string line = _getPrompt(INFO) + " " + message;
	if (_hasColor)
		std::cout << INFO_COLOR << line << RESET_COLOR << std::endl;
	else
		std::cout << line << std::endl;
	if (_hasFile)
		_appendInLogFile(line);
}

void Logger::warning(const std::string &message) const {
	if (_level > WARNING)
		return;
	std::string line = _getPrompt(WARNING) + " " + message;
	if (_hasColor)
		std::cout << WARNING_COLOR << line << RESET_COLOR << std::endl;
	else
		std::cout << line << std::endl;
	if (_hasFile)
		_appendInLogFile(line);
}

void Logger::error(const std::string &message) const {
	if (_level > ERROR)
		return;
	std::string line = _getPrompt(ERROR) + " " + message;
	if (_hasColor)
		std::cout << ERROR_COLOR << line << RESET_COLOR << std::endl;
	else
		std::cout << line << std::endl;
	if (_hasFile)
		_appendInLogFile(line);
}

void Logger::critical(const std::string &message) const {
	if (_level > CRITICAL)
		return;
	std::string line = _getPrompt(CRITICAL) + " " + message;
	if (_hasColor)
		std::cout << CRITICAL_COLOR << line << RESET_COLOR << std::endl;
	else
		std::cout << line << std::endl;
	if (_hasFile)
		_appendInLogFile(line);
}

enum Logger::level Logger::getLevel() const {
	return _level;
}

const std::string &Logger::getFile() const {
	return _file;
}

bool Logger::hasFile() const {
	return _hasFile;
}

bool Logger::hasColor() const {
	return _hasColor;
}

void Logger::setFile(const std::string &file) {
	_file = file;
	_hasFile = true;
}

void Logger::setLevel(enum Logger::level level) {
	_level = level;
}

void Logger::setHasColor(bool hasColor) {
	_hasColor = hasColor;
}

std::string Logger::_getPrompt(enum level level) {
	std::time_t now = std::time(nullptr);
	struct std::tm timeinfo;
	char buffer[50];

	localtime_r(&now, &timeinfo);
	const char *format = "[%d/%b/%Y:%H:%M:%S %z]";
	std::strftime(buffer, sizeof(buffer), format, &timeinfo);

	switch (level) {
		case DEBUG: return std::string(buffer) + " - DEBUG -";
		case INFO: return std::string(buffer) + " - INFO -";
		case WARNING: return std::string(buffer) + " - WARNING ⚠\uFE0F -";
		case ERROR: return std::string(buffer) + " - ERROR ⛔\uFE0F -";
		case CRITICAL: return std::string(buffer) + " - CRITICAL ‼\uFE0F -";
	}
	return std::string(buffer);
}

void Logger::_appendInLogFile(const std::string &line) const {
	std::ofstream logFile(_file, std::ios::app);

	if (logFile.is_open()) {
		logFile << line << std::endl;
	}
}