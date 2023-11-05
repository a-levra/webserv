#include <iostream>
#include <fstream>

#include "utils/utils.hpp"
#include "logger/logging.hpp"

bool createFile(const std::string &file, const std::string &fileContent) {
	logging::debug("Creating file : " + file);

	std::ofstream newFile(file.c_str());
	if (newFile.is_open()) {
		newFile << fileContent;
		newFile.close();
		return true;
	}
	else
		return false;
}

bool appendFile(const std::string &filename, const std::string &fileContent) {
	std::ofstream file;
	file.open(filename.c_str(), std::ios_base::app);
	if (file.is_open()) {
		file << fileContent;
		file.close();
		return true;
	}
	else
		return false;

}
