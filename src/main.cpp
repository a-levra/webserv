#include <csignal>
#include <iostream>

#include "options.hpp"
#include "logger/logging.hpp"
#include "server/Server.hpp"
#include "config/ConfigLexer.hpp"
#include "config/ConfigFactory.hpp"
#include "config/ConfigParser.hpp"

static void	setLogOptions(const Options& options);
static void printHelp();
static int 	testConfigFile(const std::string& configFile);
static int	runServer(const std::string& configFile);
static bool parseConfigFile(const ConfigLexer& lexer);
static void handleSignal(int signum);

int main(int argc, char **argv)
{
	Options options;

	if (!getOptions(argc, argv, options))
		return EXIT_FAILURE;
	setLogOptions(options);
	if (options.help) {
		printHelp();
		return EXIT_SUCCESS;
	}
	if (options.configFile.empty()) {
		logging::error("configFile not found");
		return EXIT_FAILURE;
	}
	if (options.syntax)
		return testConfigFile(options.configFile);
	else
		return runServer(options.configFile);
	return EXIT_SUCCESS;
}

static void	setLogOptions(const Options& options) {
	logging::setHasColor(options.logColor);
	logging::setLevel(options.logLevel);
	if (!options.logFile.empty())
		logging::setFile(options.logFile);
}

static void printHelp() {
	std::cout << "Webserv 1.0\n"
	   "Usage: webserv [options] *.conf\n"
	   "       webserv -c example.conf\n"
	   "Options:\n"
	   "  -h, --help                Show this help message.\n"
	   "  -l, --log-level=LEVEL     Set the logging level (LEVEL between 0 and 4).\n"
	   "                            DEBUG, INFO, WARNING, ERROR, CRITICAL\n"
	   "  -f, --log-file=FILE       Specify the log file (FILE).\n"
	   "  -c, --log-color           Enable colored logging output.\n"
	   "  -s, --syntax              Check the syntax of the configuration file."
	<< std::endl;
}

static int	testConfigFile(const std::string& configFile) {
	ConfigLexer	lexer(configFile);
	if (!parseConfigFile(lexer))
		return EXIT_FAILURE;
	logging::info("syntax is ok");
	return EXIT_SUCCESS;
}

static bool parseConfigFile(const ConfigLexer& lexer) {
	ConfigParser parser;

	if (lexer.getCodeError() != ConfigLexer::NO_ERROR) {
		logging::error("lexical: " + lexer.getError());
		return false;
	}
	parser.parseConfigFile(lexer.getConstMainContext());
	if (parser.getCodeError() != ConfigParser::NO_ERROR) {
		logging::error("syntax: " + parser.getError());
		return false;
	}
	return true;
}

static int runServer(const std::string& configFile) {
	Server			webserv;

	signal(SIGINT, &handleSignal);
    signal(SIGTERM, &handleSignal);
	{
		ConfigLexer	lexer = ConfigLexer(configFile);
		if (!parseConfigFile(lexer))
			return EXIT_FAILURE;
		logging::setHasDatetime(false);
		lexer.getMainContext().inheritDirectives();
		if (!webserv.addVirtualServers(ConfigFactory::createVirtualServers(
				lexer.getMainContext()))) {
			logging::critical("failed to launch virtual servers");
			return EXIT_FAILURE;
		}
	}
	if (!webserv.listen())
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}

static void handleSignal(int signum) {
	static_cast<void>(signum);
    logging::info("shutdown server...");
	Server::exit = true;
    signal(SIGINT, &handleSignal);
    signal(SIGTERM, &handleSignal);
}
