#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <sstream>

#define DECIMAL_BASE 10

enum colors {
	RED,
	GREEN,
	YELLOW,
	BLUE,
	GREY,
	PURPLE
};
std::string readFileToString(const std::string &filename);
void printBold(const std::string &str);
void printBoldThenThin(const std::string &str1, const std::string &str2);
void coloredLog(std::string str1, std::string str2, colors color);

template<typename T>
std::string toString(const T &value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

std::string getDate();
bool isDigitString(const std::string &s);
void ftSleep(unsigned int milliseconds);

std::vector<std::string> splitDelimiter(const std::string &s, char delimiter);
std::vector<std::string> splitDelimiter(const std::string& s, const std::string &delimiter);
std::vector<std::string> splitWhiteSpace(const std::string &s);

std::string &trim(std::string &s);
std::string &ltrim(std::string &s);
std::string &rtrim(std::string &s);

bool fileExists(const std::string &pathToFile);
void createFile(const std::string &filename, const std::string &fileContent);
void appendFile(const std::string &filename, const std::string &fileContent);

std::vector<size_t> getPositionsOfWord(const std::string &word, const std::string &src);
#endif //UTILS_HPP
