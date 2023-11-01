#include <fstream>
#include <iostream>

std::string readFileToString(const std::string& filename, bool & success) {
	std::ifstream file(filename.c_str());

	if (!file.is_open()) {
		std::cerr << "Error: Unable to open file " << filename << std::endl;
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