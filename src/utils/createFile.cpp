#include <iostream>
#include <fstream>

#include "utils/utils.hpp"
#include "logger/logging.hpp"

bool createFile(const std::string &filename, const std::string &fileContent) {
	logging::debug("Creating file : " + filename);
//	coloredLog("File content: ", fileContent, GREEN);

	std::ofstream file(("uploadedFiles/" + filename).c_str());
	if (file.is_open()) {
		file << fileContent;
		file.close();
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
