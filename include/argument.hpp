/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argument.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdameros <tdameros@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 19:19:00 by tdameros          #+#    #+#             */
/*   Updated: 2023/10/27 19:19:00 by tdameros         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef INC_42_WEBSERV_INCLUDE_ARGUMENT_HPP_
# define INC_42_WEBSERV_INCLUDE_ARGUMENT_HPP_

#include <string>
#include "logger/Logger.hpp"

struct Flag {
  std::string	flagName;
  bool			requiresArgument;
  char			shortcut;
};

Flag parseCommandLineArgument(const std::string& argument, std::string& flagArgument);

#endif