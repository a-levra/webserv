#ifndef ARGUMENT_HPP
# define ARGUMENT_HPP

#include <string>

#include "logger/Logger.hpp"

struct Flag {
  std::string	flagName;
  bool			requiresArgument;
  char			shortcut;
};

Flag parseCommandLineArgument(const std::string& argument,
							  std::string& flagArgument);

#endif