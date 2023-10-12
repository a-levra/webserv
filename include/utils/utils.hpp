#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
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
void coloredLog(const std::string &str1, const std::string &str2, colors color);
#include <sstream>

template<typename T>
std::string toString(const T &value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

std::string getDate();
#endif //UTILS_HPP
