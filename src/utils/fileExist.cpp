#include "utils/utils.hpp"
#include <sys/stat.h>

bool fileExists(const std::string &pathToFile) {
	struct stat buffer;
	return (stat(pathToFile.c_str(), &buffer) == 0);
}