#include "cgi/CGI.hpp"

#include <unistd.h>
#include <sys/wait.h>

#include <algorithm>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>

#include "logger/logging.hpp"


CGI::CGI() {

}

CGI::CGI(const CGI &other) {
	*this = other;
}

CGI::~CGI() {

}

CGI::codeError CGI::execute(const HttpResponse& response) {
  	if (!_writeInTmpIN(response.getRequest().getBody())) {
		return FAILED;
	}
	char** envp = _initEnv(response);
	pid_t pidExec = _runExecutable(response.getCGIExtension().second,
								   response.getCGIPath(),
								   response.getCGIFile(), envp);
	_deleteEnv(envp);
	if (pidExec == -1)
		return FAILED;
	pid_t pidTimeOut = _runTimeout(CGI_TIMEOUT_MS);
	if (pidTimeOut == -1) {
		kill(pidExec, SIGKILL);
		waitpid(-1, NULL, WUNTRACED);
		return FAILED;
	}
	std::pair<pid_t, int> firstChild = _waitFirstChild(pidExec, pidTimeOut);
	if (!_readTmpFiles())
		return FAILED;
	if (firstChild.first == pidExec && firstChild.second == 0)
		return SUCCESSFUL;
	else if (firstChild.first == pidExec)
		return FAILED;
	return TIMEOUT;
}

pid_t CGI::_runExecutable(const std::string &interpreter, const std::string &path, const std::string &executable, char **envp) {
	pid_t pidExec = fork();
	if (pidExec == -1) {
		logging::warning("Fork executable failed");
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

pid_t CGI::_runTimeout(size_t ms) {
	pid_t pidTimeOut = fork();
	if (pidTimeOut == -1) {
		logging::warning("Fork timeout failed");
		return pidTimeOut;
	}
	else if (pidTimeOut == 0) {
		ftSleep(ms);
		std::exit(EXIT_SUCCESS);
	}
	return pidTimeOut;
}

std::pair<pid_t, int> CGI::_waitFirstChild(pid_t child1, pid_t child2) {
	int statusCode = 0;
	pid_t firstChildPid = waitpid(-1, &statusCode, WUNTRACED);

	if (firstChildPid == child1) {
		if (kill(child2,SIGKILL) == -1) {
			logging::error("Kill failed");
		}
	}
	else {
		if (kill(child1, SIGKILL) == -1) {
			logging::error("Kill failed");
		}
	}
	waitpid(-1, NULL, WUNTRACED);
	return std::make_pair(firstChildPid, WEXITSTATUS(statusCode));
}

char **CGI::_initEnv(const HttpResponse& response) {
	const std::map<std::string, std::string>& headers = response.getRequest().getHeaders();
	char** envp = new char*[headers.size() + NB_DEFAULT_ENV_VAR + 1];

	_setDefaultEnv(response, envp);
	size_t envpIndex = NB_DEFAULT_ENV_VAR;
	std::map<std::string, std::string>::const_iterator it;
	for (it = headers.begin(); it != headers.end(); it++) {
		envp[envpIndex] = _getEnvVariable("HTTP_" + it->first, it->second);
		envpIndex++;
	}
	envp[envpIndex] = NULL;
	return envp;
}

void CGI::_setDefaultEnv(const HttpResponse &response, char **envp) {
	const std::map<std::string, std::string>& headers = response.getRequest().getHeaders();
	size_t envpIndex = 0;
	envp[envpIndex] = _getEnvVariable("PATH_INFO", response.getCGIPathInfo());
	envpIndex++;
	envp[envpIndex] = _getEnvVariable("REQUEST_METHOD",
									  response.getRequest().getMethod());
	envpIndex++;
	if (headers.find("Content-Length") == headers.end()) {
		envp[envpIndex] = _getEnvVariable("CONTENT_LENGTH", "0");
	}
	else {
		envp[envpIndex] = _getEnvVariable("CONTENT_LENGTH",
										  headers.find("Content-Length")->second);
	}
	envpIndex++;
	if (headers.find("Content-Type") == headers.end()) {
		envp[envpIndex] = _getEnvVariable("CONTENT_TYPE", "text/html");
	}
	else {
		envp[envpIndex] = _getEnvVariable("CONTENT_TYPE",
										  headers.find("Content-Type")->second);
	}
}


char *CGI::_getEnvVariable(const std::string& key, const std::string& value) {
	std::string newKey = key;
	std::transform(newKey.begin(), newKey.end(), newKey.begin(), ::toupper);
	std::replace(newKey.begin(), newKey.end(), '-', '_');
	newKey += "=" + value;
	char* envVariable = new char[newKey.length() + 1];
	std::strcpy(envVariable, newKey.c_str());
	return envVariable;
}

void  CGI::_deleteEnv(char **env) {
	if (env == NULL) return;
	for (size_t i = 0; env[i] != NULL; i++) {
		delete[] env[i];
	}
	delete[] env;
}

bool CGI::_writeInTmpIN(const std::string &content) {
	std::ofstream fileIn(PATH_CGI_IN);

	if (fileIn.is_open()) {
		fileIn << content;
		return true;
	}
	logging::warning("open tmpIN failed");
	return false;
}

bool CGI::_readTmpFiles() {
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
		logging::warning("open tmpOut failed");
		return false;
	}
	content = "";
	if (tmpErr.is_open()) {
		while (getline(tmpErr, line)) {
			content += line;
		}
		_stderr = content;
	} else {
		logging::warning("open tmpErr failed");
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

CGI &CGI::operator=(const CGI &other) {
	if (this == &other)
		return *this;
	_stdout = other._stdout;
	_stderr = other._stderr;
	_codeError = other._codeError;
	return *this;
}