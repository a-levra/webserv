#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <string>

#include "argument.hpp"
#include "logger/logging.hpp"
#include "utils/utils.hpp"
#include "options.hpp"

static struct Options getDefaultOptions();
static bool	parseArgument(const std::string& argument, Options& options);
static bool parseLogLevel(const std::string &flagContent,
						  enum Logger::level &level);
static bool isConfigFile(const std::string& configFile);

bool getOptions(int argc, char **argv, Options &options) {
	options = getDefaultOptions();

	for (int i = 1; i < argc; i++) {
		if (!parseArgument(argv[i], options))
			return false;
	}
	return true;
}

static bool	parseArgument(const std::string& argument, Options& options) {
	std::string flagArgument;
	struct Flag flag = parseCommandLineArgument(argument, flagArgument);
	if (flag.shortcut == 'l') {
		if (!parseLogLevel(flagArgument, options.logLevel)) {
			logging::error("invalid loglevel argument");
			return false;
		}
	} else if (flag.shortcut == 'f') {
		if (flagArgument.empty()) {
			logging::error("invalid logfile argument");
			return false;
		}
		options.logFile = flagArgument;
	} else if (flag.shortcut == 'c')
		options.logColor = true;
	else if (flag.shortcut == 'h')
		options.help = true;
	else if (flag.shortcut == 's')
		options.syntax = true;
	else {
		if (isConfigFile(flagArgument) && options.configFile.empty())
			options.configFile = flagArgument;
		else {
			logging::error("\"" + flagArgument + "\" invalid argument");
			return false;
		}
	}
	return true;
}

static struct Options getDefaultOptions() {
	struct Options options;

	options.logFile = "";
	options.configFile = "";
	options.logLevel = Logger::WARNING;
	options.logColor = false;
	options.help = false;
	options.syntax = false;
	return options;
}

static bool parseLogLevel(const std::string &flagContent, enum Logger::level &level) {
	if (flagContent.empty())
		return false;
	char *endptr;
	long codeLevel = std::strtol(flagContent.c_str(), &endptr, DECIMAL_BASE);
	if (errno == ERANGE || *endptr != '\0'
		|| codeLevel < Logger::DEBUG || codeLevel > Logger::CRITICAL)
		return false;
	level = static_cast<enum Logger::level>(codeLevel);
	return true;
}

static bool isConfigFile(const std::string& configFile) {
	std::string::size_type pos = configFile.rfind(CONFIG_FILE_EXTENSION);
	return (pos != std::string::npos
			&& pos == configFile.length() - std::strlen(CONFIG_FILE_EXTENSION));
}