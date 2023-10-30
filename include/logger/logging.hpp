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

#ifndef LOGGING_HPP
# define LOGGING_HPP

#include "logger/Logger.hpp"

namespace logging {

	static Logger logger;

	void log(enum Logger::level level, const std::string &message);
	void debug(const std::string &message);
	void info(const std::string& message);
	void warning(const std::string& message);
	void error(const std::string &message);
	void critical(const std::string &message);

	enum Logger::level getLevel();
	const std::string& getFile();
	bool hasFile();
	bool hasColor();
	bool hasDatetime();

	void setFile(const std::string& file);
	void setLevel(enum Logger::level level);
	void setHasColor(bool hasColor);
	void setHasDatetime(bool hasDatetime);

}

#endif
