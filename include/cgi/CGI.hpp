#ifndef CGI_HPP
#define CGI_HPP

# include <iostream>

# include "virtualServer/Location.hpp"
# include "HttpMessages/HttpResponse.hpp"

# define PATH_CGI_OUT "/tmp/webserv_cgi_stdout"
# define PATH_CGI_ERR "/tmp/webserv_cgi_stderr"
# define PATH_CGI_IN "/tmp/webserv_cgi_stdin"

# define CGI_TIMEOUT_MS 5000
# define NB_DEFAULT_ENV_VAR 4

class CGI {

public:

	enum codeError
	{
		SUCCESSFUL,
		FAILED,
		TIMEOUT,
	};

	CGI();
	CGI(const CGI &other);
	~CGI();

	CGI &operator=(const CGI &other);

	CGI::codeError		execute(const HttpResponse& response);

	const std::string&	getStdErr() const;
	const std::string&	getStdOut() const;
	codeError			getCodeError() const;

private:
	pid_t 	_runExecutable(const std::string& interpreter, const std::string& path, const std::string& executable, char **envp);
	pid_t	_runTimeout(size_t ms);
	std::pair<pid_t, int>	_waitFirstChild(pid_t child1, pid_t child2);
	char**	_initEnv(const HttpResponse& response);
	void	_setDefaultEnv(const HttpResponse& response, char **envp);
	char*	_getEnvVariable(const std::string& key, const std::string& value);
	void    _deleteEnv(char** env);
	bool	_writeInTmpIN(const std::string& content);
	bool	_readTmpFiles();

	std::string	_stdout;
	std::string	_stderr;
	codeError	_codeError;
};

#endif
