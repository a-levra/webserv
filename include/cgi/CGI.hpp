#ifndef CGI_HPP
#define CGI_HPP

# include <iostream>

# include "virtualServer/Location.hpp"
# include "HttpMessages/HttpResponse.hpp"

# define PATH_CGI_OUT "/tmp/webserv_cgi_stdout"
# define PATH_CGI_ERR "/tmp/webserv_cgi_stderr"
# define PATH_CGI_IN "/tmp/webserv_cgi_stdin"

# define CGI_TIMEOUT_MS 5000

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

	CGI::codeError	execute(const HttpResponse& request);
	bool	executeBinary(const std::string& path, const std::string& script);
	pid_t 	runExecutable(const std::string& interpreter, const std::string& path, const std::string& executable, char **envp);
	pid_t	runTimeout(size_t ms);
	std::pair<pid_t, int>	waitFirstChild(pid_t child1, pid_t child2);
	char**	initEnv(const HttpResponse& request);
	char*	getEnvVariable(const std::string& key, const std::string& value);
    void    deleteEnv(char** env);
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
};

#endif
