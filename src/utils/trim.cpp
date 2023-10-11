#include "utils.hpp"

#include <algorithm>


std::string &trim(std::string &s)
{
	return (ltrim(rtrim(s)));
}

std::string &ltrim(std::string &s)
{
	s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), std::isspace));
	return s;
}

std::string &rtrim(std::string &s)
{
	s.erase(std::find_if_not(s.rbegin(), s.rend(), std::isspace).base(), s.end());
	return s;
}
