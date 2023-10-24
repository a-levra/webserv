#include "server/Server.hpp"
#include "config/ConfigLexer.hpp"
#include "config/ConfigFactory.hpp"
#include "config/ConfigParser.hpp"

#include <csignal>
#include <iostream>

static bool	isParsingFlag(int argc, char *argv[]);
static int 	testConfigFile(const std::string& configFile);
static bool parseConfigFile(const ConfigLexer& lexer);
static int	runServer(const std::string& configFile);
static void handleSignal(int signum);


#include "cgi/CGI.hpp"
int main(int argc, char **argv)
{
	(void) argc; (void) argv;
	(void) isParsingFlag; (void) testConfigFile; (void) parseConfigFile;
	(void) runServer; (void) handleSignal;
	HttpRequest request("GET /cgibin/truc/bidule/test.py/ HTTP/1.1\r\nHost: test\r\n\r\n");
	Location location("/cgibin/");

	location.setRoot("cgi-bin");
	std::vector<std::string> indexes;
	indexes.push_back("main.py");
	location.setIndex(indexes);
	request.parse();
	std::cout << request.checkValidity() << std::endl;

	CGI	cgi;

	if (cgi.execute(request, location) == CGI::SUCCESSFUL) {
		std::cout << "STDOUT:" << std::endl << cgi.getStdOut() << std::endl;
	}
	else if (cgi.getCodeError() == CGI::FAILED) {
		std::cout << "STDERR:" << std::endl << cgi.getStdOut() << std::endl;
	}
	else
		std::cout << "Timeout..." << std::endl;

//	if (!isParsingFlag(argc, argv) && argc != 2) {
//		std::cerr << "Usage: ./webserv *.conf" << std::endl;
//		return EXIT_FAILURE;
//	}
//	if (isParsingFlag(argc, argv))
//		return testConfigFile(argv[2]);
//	else
//		return runServer(argv[1]);
//	return EXIT_FAILURE;
}

static bool	isParsingFlag(int argc, char *argv[]) {
	return (argc == 3 && std::string(argv[1]) == "-t");
}

static int	testConfigFile(const std::string& configFile) {
	ConfigLexer	lexer(configFile);
	if (!parseConfigFile(lexer))
		return EXIT_FAILURE;
	std::cout << "Syntax is ok" << std::endl;
	return EXIT_SUCCESS;
}

static bool parseConfigFile(const ConfigLexer& lexer) {
	ConfigParser parser;

	if (lexer.getCodeError() != ConfigLexer::NO_ERROR) {
		std::cerr << "Lexical Error: " <<  lexer.getError() << std::endl;
		return false;
	}
	parser.parseConfigFile(lexer.getConstMainContext());
	if (parser.getCodeError() != ConfigParser::NO_ERROR) {
		std::cerr << "Syntax Error: " <<  parser.getError() << std::endl;
		return false;
	}
	return true;
}

static int runServer(const std::string& configFile) {
	Server			webserv;

	signal(SIGINT, &handleSignal);
	{
		ConfigLexer	lexer = ConfigLexer(configFile);
		if (!parseConfigFile(lexer))
			return EXIT_FAILURE;
		lexer.getMainContext().inheritDirectives();
		if (!webserv.addVirtualServers(ConfigFactory::createVirtualServers(
				lexer.getMainContext()))) {
			std::cerr << "webserv: failed to launch virtual servers" << std::endl;
			return EXIT_FAILURE;
		}
	}
	if (!webserv.listen())
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}

static void handleSignal(int signum) {
	static_cast<void>(signum);
	Server::exit = true;
}