/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logging.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 23:39:00 by tdameros          #+#    #+#             */
/*   Updated: 2023/10/27 23:39:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "logger/logging.hpp"

namespace logging {

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

	bool hasDatetime() {
		return logger.hasDatetime();
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

	void setHasDatetime(bool hasDatetime) {
		logger.setHasDatetime(hasDatetime);
	}

}

