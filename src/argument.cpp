#include "argument.hpp"

#include <string>

static bool isFlagString(const std::string& argument, const Flag& flag);
static Flag parseFlagArgument(const std::string& argument, const Flag& flag, std::string& flagArgument);

Flag parseCommandLineArgument(const std::string& argument, std::string& flagArgument) {
	const Flag flags[] = {
		{ "log-level", true, 'l' },
		{ "log-file", true, 'f' },
		{ "log-color", false, 'c' },
		{ "help", false, 'h' },
		{ "syntax", false, 's' }
	};
	int nbFlags = sizeof(flags) / sizeof(Flag);
	for (int i = 0; i < nbFlags; i++) {
		if (isFlagString(argument, flags[i])) {
			return parseFlagArgument(argument, flags[i], flagArgument);
		}
	}
	flagArgument = argument;
	return (struct Flag){"?", false, '?'};
}

static bool isFlagString(const std::string& argument, const Flag& flag) {
	if (flag.requiresArgument)
		return (argument.find("-" + std::string(1, flag.shortcut)) == 0 || argument.find("--" + flag.flagName) == 0);
	return (argument == "-" + std::string(1, flag.shortcut) ||
		argument == "--" + flag.flagName);
}

static Flag parseFlagArgument(const std::string& argument, const Flag& flag, std::string& flagArgument) {
	if (!flag.requiresArgument) {
		flagArgument = "";
		return flag;
	}
	if (argument.find("--" + flag.flagName) == 0) {
		if (argument[flag.flagName.length() + 2] == '=')
			flagArgument = argument.substr(flag.flagName.length() + 3);
		else
			flagArgument = argument.substr(flag.flagName.length() + 2);
		return flag;
	}
	if (argument.find("-" + std::string(1, flag.shortcut)) == 0) {
		if (argument[2] == '=')
			flagArgument = argument.substr(3);
		else
			flagArgument = argument.substr(2);
		return flag;
	}
	flagArgument = argument;
	return (struct Flag){"?", false, '?'};
}
