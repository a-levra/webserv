#include <fstream>
#include <iostream>

std::string readFileToString(const std::string& filename) {
	std::ifstream file(filename.c_str());

	if (!file.is_open()) {
		std::cerr << "Error: Unable to open file " << filename << std::endl;
		return "";
	}

	std::string file_contents;
	std::string line;

	while (std::getline(file, line)) {
		file_contents += line + '\n';
	}

	file.close();
	return file_contents;
}