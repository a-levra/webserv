#include "cgi/CGI.hpp"

#include "unistd.h"

#include <fstream>
#include <iostream>
#include <iosfwd>
#include <cstdio>
#include <csignal>


CGI::CGI() {

}

CGI::CGI(const CGI &other) {
	*this = other;
}

CGI::~CGI() {

}

bool CGI::execute(const HttpRequest& request, const Location& location) {
	static_cast<void>(_codeError);
	static_cast<void>(_stderr);
	static_cast<void>(_stdout);
	static_cast<void>(location);

	// http://localhost/cgin-bin/hello.py/hello-world/
	std::string root = "cgi-bin";
	std::string exec = "hello.py";
	std::string uri = "/";
	std::string pathInfo = "/hello-world/";
	std::string python = "/usr/bin/python3";

	char** envp = initEnv(request.getHeaders(), pathInfo);
	writeInTmpIN("Hello from C++!");

	pid_t pidExec = runExecutable(python, root, exec, envp);
	if (pidExec == -1)
		return false;
	pid_t pidTimeOut = runTimeout(3000);
	if (pidTimeOut == -1)
		return false;
	std::pair<pid_t, int> firstChild = waitFirstChild(pidExec, pidTimeOut);
	readTmpFiles();
	if (firstChild.first == pidExec && firstChild.second == 0)
		return SUCCESSFUL;
	else if (firstChild.first == pidExec)
		return FAILED;
	return TIMEOUT;
}

pid_t CGI::runExecutable(const std::string &interpreter, const std::string &path, const std::string &executable, char **envp) {
	pid_t pidExec = fork();
	if (pidExec == -1) {
		return pidExec;
	}
	else if (pidExec == 0) {
		std::freopen(PATH_CGI_IN, "r", stdin);
		std::freopen(PATH_CGI_OUT, "w", stdout);
		std::freopen(PATH_CGI_ERR, "w", stderr);
		if (chdir(path.c_str()) == -1)
			std::exit(EXIT_FAILURE);
		char *const arguments[] = {
			(char *)interpreter.c_str(),
			(char *)executable.c_str(),
			NULL
		};
		execve(interpreter.c_str(), arguments, envp);
		std::exit(EXIT_FAILURE);
	}
	return pidExec;
}

pid_t CGI::runTimeout(size_t ms) {
	pid_t pidTimeOut = fork();
	if (pidTimeOut == -1) {
		perror("fork");
	}
	else if (pidTimeOut == 0) {
		ftSleep(ms);
		std::exit(EXIT_SUCCESS);
	}
	return pidTimeOut;
}

std::pair<pid_t, int> CGI::waitFirstChild(pid_t child1, pid_t child2) {
	int statusCode = 0;
	pid_t firstChildPid = waitpid(-1, &statusCode, WUNTRACED);

	if (firstChildPid == child1)
		kill(child2,SIGKILL);
	else
		kill(child1, SIGKILL);
	waitpid(-1, NULL, WUNTRACED);
	return std::make_pair(firstChildPid, WEXITSTATUS(statusCode));
}

char **CGI::initEnv(const std::map<std::string, std::string>& headers, const std::string &pathInfo) {
	char** envp = new char*[headers.size() + 2];

	size_t envpIndex = 0;
	envp[envpIndex] = getEnvVariable("PATH_INFO=" + pathInfo);
	envpIndex++;
	std::map<std::string, std::string>::const_iterator it;
	for (it = headers.begin(); it != headers.end(); it++) {
		envp[envpIndex] = getEnvVariable("HTTP_" + it->first + "=" + it->second);
		envpIndex++;
	}
	envp[envpIndex] = NULL;
	return envp;
}

char *CGI::getEnvVariable(std::string s) {
	std::transform(s.begin(), s.end(), s.begin(), ::toupper);
	std::replace(s.begin(), s.end(), '-', '_');
	char* envVariable = new char[s.length() + 1];
	std::strcpy(envVariable, s.c_str());
	return envVariable;
}

bool CGI::writeInTmpIN(const std::string &content) {
	std::ofstream fileIn(PATH_CGI_IN);

	if (fileIn.is_open()) {
		fileIn << content;
		return true;
	}
	return false;
}

bool CGI::readTmpFiles() {
	std::ifstream tmpOut(PATH_CGI_OUT);
	std::ifstream tmpErr(PATH_CGI_ERR);

	if (tmpOut.is_open())
		tmpOut >> _stdout;
	if (tmpErr.is_open())
		tmpErr >> _stderr;
	return true;
}

const std::string &CGI::getStdOut() const {
	return _stdout;
}

const std::string &CGI::getStdErr() const {
	return _stderr;
}

CGI::codeError CGI::getCodeError() const {
	return _codeError;
}

std::string::size_type CGI::findEndBinaryPos(const std::string &s) {
	std::vector<std::string> extensions;
	extensions.push_back(".py");
	extensions.push_back(".php");

	std::string::size_type binaryPos = std::string::npos;
	std::vector<std::string>::const_iterator it;
	for (it = extensions.begin(); it != extensions.end(); it++) {
		std::string::size_type pos = s.find(*it);
		// TODO : query check ?
		if (pos != std::string::npos && (s[pos + it->length()] == '/'
										|| s[pos + it->length()] == '?'
										|| s[pos + it->length()] == '\0'))
			binaryPos = pos + it->length();
	}
	return binaryPos;
}

CGI &CGI::operator=(const CGI &other) {
	if (this == &other)
		return *this;
	return *this;
}