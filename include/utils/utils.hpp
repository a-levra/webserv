#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <sstream>

#define DECIMAL_BASE 10

#define BOLD "\033[1m"
#define THIN "\033[0m"
#define B_RED BOLD "\033[1;31m"
#define B_GREEN BOLD "\033[1;32m"
#define B_YELLOW BOLD "\033[1;33m"
#define B_BLUE BOLD "\033[1;34m"
#define B_PURPLE BOLD "\033[1;35m"
#define COLOR_RESET "\033[0m"

std::string readFileToString(const std::string& filename, bool & success);
void printBold(const std::string &str);
void printBoldThenThin(const std::string &str1, const std::string &str2);

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
std::string& trimCharset(std::string& s, const std::string& charset);
std::string& ltrimCharset(std::string& s, const std::string& charset);
std::string& rtrimCharset(std::string& s, const std::string& charset);

bool fileExists(const std::string &pathToFile);
bool createFile(const std::string &file, const std::string &fileContent);
bool appendFile(const std::string &filename, const std::string &fileContent);

std::vector<size_t> getPositionsOfWord(const std::string &word, const std::string &src);

bool isDirectory(const std::string& path);

#endif
