#include <iostream>
#include <fstream>

#include "utils/utils.hpp"

void createFile(const std::string &filename, const std::string &fileContent) {
	coloredLog("Creating file: ", filename, GREEN);
	coloredLog("File content: ", fileContent, GREEN);
	std::ofstream file;
	file.open(("uploaded_files/" + filename).c_str());
	file << fileContent;
	file.close();
}

void appendFile(const std::string &filename, const std::string &fileContent) {
	std::ofstream file;
	file.open(filename.c_str(), std::ios_base::app);
	file << fileContent;
	file.close();
}
