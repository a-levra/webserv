/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   options.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 22:28:00 by tdameros          #+#    #+#             */
/*   Updated: 2023/10/27 22:28:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef INC_42_WEBSERV_INCLUDE_OPTIONS_HPP_
# define INC_42_WEBSERV_INCLUDE_OPTIONS_HPP_

#include "logger/Logger.hpp"

#define CONFIG_FILE_EXTENSION ".conf"

struct Options {
	std::string			logFile;
	std::string 		configFile;
	enum Logger::level	logLevel;
	bool				logColor;
	bool				help;
	bool				syntax;
};

bool getOptions(int argc, char **argv, Options &options);
#endif