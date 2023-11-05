#include <sys/stat.h>

#include <string>

bool isDirectory(const std::string& path) {
	struct stat	pathStat;

	if (stat(path.c_str(), &pathStat) == -1) {
		return false;
	}
	return (S_ISDIR(pathStat.st_mode));
}
