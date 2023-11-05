#include "cgi/CGI.hpp"

#include <unistd.h>
#include <sys/wait.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iosfwd>
#include <cstdio>
#include <csignal>
#include <cstdlib>
#include <cstring>


CGI::CGI() {

}

CGI::CGI(const CGI &other) {
	*this = other;
}

CGI::~CGI() {

}

CGI::codeError CGI::execute(const HttpResponse& response) {
  	if (!writeInTmpIN(response.getRequest().getBody())) {
		return FAILED;
	}
	char** envp = initEnv(response);
	pid_t pidExec = runExecutable(response.getCGIExtension().second,
								  response.getCGIPath(),
								  response.getCGIFile(), envp);
	deleteEnv(envp);
	if (pidExec == -1)
		return FAILED;
	pid_t pidTimeOut = runTimeout(CGI_TIMEOUT_MS);
	if (pidTimeOut == -1)
		return FAILED;
	std::pair<pid_t, int> firstChild = waitFirstChild(pidExec, pidTimeOut);
	if (!readTmpFiles())
		return FAILED;
	if (firstChild.first == pidExec && firstChild.second == 0)
		return SUCCESSFUL;
	else if (firstChild.first == pidExec)
		return FAILED;
	return TIMEOUT;
}

pid_t CGI::runExecutable(const std::string &interpreter, const std::string &path, const std::string &executable, char **envp) {
	pid_t pidExec = fork();
	if (pidExec == -1) {
		std::cout << "fork failed" << std::endl;
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

char **CGI::initEnv(const HttpResponse& request) {
	const std::map<std::string, std::string>& headers = request.getRequest().getHeaders();
	char** envp = new char*[headers.size() + 5];

	size_t envpIndex = 0;
	envp[envpIndex] = getEnvVariable("PATH_INFO", request.getCGIPathInfo());
	envpIndex++;
  	envp[envpIndex] = getEnvVariable("REQUEST_METHOD",  request.getRequest().getMethod());
  	envpIndex++;
	envp[envpIndex] = getEnvVariable("CONTENT_LENGTH",  headers.find("Content-Length")->second);
	envpIndex++;
	envp[envpIndex] = getEnvVariable("CONTENT_TYPE",  headers.find("Content-Type")->second);
	envpIndex++;
	std::map<std::string, std::string>::const_iterator it;
	for (it = headers.begin(); it != headers.end(); it++) {
		envp[envpIndex] = getEnvVariable("HTTP_" + it->first, it->second);
		envpIndex++;
	}
	envp[envpIndex] = NULL;
	return envp;
}

char *CGI::getEnvVariable(const std::string& key, const std::string& value) {
	std::string newKey = key;
	std::transform(newKey.begin(), newKey.end(), newKey.begin(), ::toupper);
	std::replace(newKey.begin(), newKey.end(), '-', '_');
	newKey += "=" + value;
	char* envVariable = new char[newKey.length() + 1];
	std::strcpy(envVariable, newKey.c_str());
	return envVariable;
}

void  CGI::deleteEnv(char **env) {
	if (env == NULL) return;
	for (size_t i = 0; env[i] != NULL; i++) {
		delete[] env[i];
	}
	delete[] env;
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
	std::string content;
	std::string line;

	if (tmpOut.is_open()) {
		while (getline(tmpOut, line)) {
			content += line + "\n";
		}
		_stdout = content;
	} else {
		return false;
	}
	content = "";
	if (tmpErr.is_open()) {
		while (getline(tmpErr, line)) {
			content += line;
		}
		_stderr = content;
	} else {
		return false;
	}
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