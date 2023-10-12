#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>

#define DECIMAL_BASE 10

bool	isDigitString(const std::string& s);

std::string readFileToString(const std::string& filename);

std::vector<std::string>	splitDelimiter(const std::string& s, char delimiter);
std::vector<std::string>	splitWhiteSpace(const std::string& s);

std::string &trim(std::string &s);
std::string &ltrim(std::string &s);
std::string &rtrim(std::string &s);

#endif
