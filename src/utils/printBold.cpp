#include <iostream>

void printBold(const std::string& str)
{
	std::cout << "\033[1m" << str << "\033[0m";
}

void printBoldThenThin(const std::string& str1, const std::string& str2 )
{
	std::cout << "\033[1m" << str1 << "\033[0m" << str2 << std::endl;
}