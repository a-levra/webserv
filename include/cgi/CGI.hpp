#ifndef INC_42_WEBSERV_SRC_CGI_CGI_HPP_
# define INC_42_WEBSERV_SRC_CGI_CGI_HPP_

# include <iostream>

# include "virtualServer/Location.hpp"
# include "HttpMessages/HttpRequest.hpp"

# define PATH_CGI_OUT "/tmp/webserv_cgi_stdout"
# define PATH_CGI_ERR "/tmp/webserv_cgi_stderr"
# define PATH_CGI_IN "/tmp/webserv_cgi_stdin"

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

	bool	execute(const HttpRequest& request, const Location& location);
	bool	executeBinary(const std::string& path, const std::string& script);
	pid_t 	runExecutable(const std::string& interpreter, const std::string& path, const std::string& executable, char **envp);
	pid_t	runTimeout(size_t ms);
	std::pair<pid_t, int>	waitFirstChild(pid_t child1, pid_t child2);
	char**	initEnv(const std::map<std::string, std::string>& headers, const std::string& pathInfo);
	char*	getEnvVariable(const std::string s);
	std::string::size_type findEndBinaryPos(const std::string& s);
	bool	writeInTmpIN(const std::string& content);
	bool	readTmpFiles();

	const std::string& getStdErr() const;
  	const std::string& getStdOut() const;
	codeError			getCodeError() const;

private:
	std::string	_stdout;
	std::string	_stderr;
  	codeError	_codeError;
//	char**		_env;
};

#endif
