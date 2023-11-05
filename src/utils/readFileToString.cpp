#include <fstream>

#include "logger/logging.hpp"

std::string readFileToString(const std::string& filename, bool & success) {
	std::ifstream file(filename.c_str());

	if (!file.is_open()) {
		logging::error("Unable to open file " + filename);
		success = false;
		return "";
	}

	std::string file_contents;
	std::string line;

	while (std::getline(file, line)) {
		file_contents += line + '\n';
	}

	file.close();
	success = true;
	return file_contents;
}