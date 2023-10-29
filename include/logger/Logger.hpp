#ifndef INC_42_WEBSERV_SRC_LOGGER_LOGGER_HPP_
# define INC_42_WEBSERV_SRC_LOGGER_LOGGER_HPP_

#include <string>


#define RESET_COLOR "\033[0m"
#define START_COLOR(style, color) ("\033["style";"color"m")

#define STR_GREY "30"
#define STR_RED "31"
#define STR_GREEN "32"
#define STR_YELLOW "33"
#define STR_BLUE "34"
#define STR_PURPLE "35"

#define DEBUG_COLOR START_COLOR("0", STR_GREEN)
#define INFO_COLOR START_COLOR("0", STR_BLUE)
#define WARNING_COLOR START_COLOR("0", STR_YELLOW)
#define ERROR_COLOR START_COLOR("0", STR_RED)
#define CRITICAL_COLOR START_COLOR("1", STR_RED)

class Logger {

public:

  	enum level {
		DEBUG,
		INFO,
		WARNING,
		ERROR,
		CRITICAL,
	};

	enum colors {
	  	GREY = 30,
		RED = 31,
		GREEN = 32,
		YELLOW = 33,
		BLUE = 34,
		PURPLE = 35,
	};

	Logger();
	Logger(const Logger &other);
	~Logger();

	Logger &operator=(const Logger &other);

	void	log(enum level, const std::string& message) const;
	void	debug(const std::string& message) const;
	void	info(const std::string& message) const;
	void	warning(const std::string& message) const;
	void	error(const std::string& message) const;
	void	critical(const std::string& message) const;

  	enum level			getLevel() const;
	const std::string&	getFile() const;
  	bool				hasFile() const;
	bool				hasColor() const;
	bool				hasDatetime() const;

	void	setFile(const std::string& file);
	void	setLevel(enum level level);
	void	setHasColor(bool hasColor);
	void	setHasDatetime(bool hasDatetime);

private:
	std::string _getPrompt(enum level level) const;
	void		_appendInLogFile(const std::string& line) const;

	std::string	_file;
	bool		_hasFile;
	bool		_hasColor;
	bool		_hasDatetime;
  	enum level	_level;
};

#endif
