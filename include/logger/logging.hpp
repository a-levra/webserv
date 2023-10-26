/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logging.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 00:17:00 by tdameros          #+#    #+#             */
/*   Updated: 2023/10/27 00:17:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef INC_42_WEBSERV_INCLUDE_LOGGER_LOGGING_HPP_
# define INC_42_WEBSERV_INCLUDE_LOGGER_LOGGING_HPP_

#include "logger/Logger.hpp"

namespace logging {

	static Logger logger;

	void log(enum Logger::level level, const std::string &message) {
		logger.log(level, message);
	}

	void debug(const std::string &message) {
		logger.debug(message);
	}

	void info(const std::string& message) {
		logger.info(message);
	}

	void warning(const std::string& message) {
		logger.warning(message);
	}

	void error(const std::string &message) {
		logger.error(message);
	}

	void critical(const std::string &message) {
		logger.critical(message);
	}

	enum Logger::level getLevel() {
		return logger.getLevel();
	}

	const std::string& getFile() {
		return logger.getFile();
	}

	bool hasFile() {
		return logger.hasFile();
	}

	bool hasColor() {
		return logger.hasColor();
	}

	void setFile(const std::string& file) {
		logger.setFile(file);
	}

	void setLevel(enum Logger::level level) {
		logger.setLevel(level);
	}

	void setHasColor(bool hasColor) {
		logger.setHasColor(hasColor);
	}

}

#endif
