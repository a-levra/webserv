#include "utils/utils.hpp"

#include <algorithm>

std::string &trim(std::string &s) {
	return (ltrim(rtrim(s)));
}

std::string &ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun(isspace))));
	return s;
}

std::string &rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun(isspace))).base(), s.end());
	return s;
}

std::string&	trimCharset(std::string& s, const std::string& charset) {
	return ltrimCharset(rtrimCharset(s, charset), charset);
}

std::string&	ltrimCharset(std::string& s, const std::string& charset) {
	s.erase(0, s.find_first_not_of(charset));
	return s;
}

std::string& rtrimCharset(std::string& s, const std::string& charset) {
	s.erase(s.find_last_not_of(charset) + 1);
	return s;
}


