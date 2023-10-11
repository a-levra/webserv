#include "utils.hpp"

std::vector<std::string> splitDelimiter(const std::string& s, char delimiter) {
	std::vector<std::string> result;
	std::string::size_type start = 0;
	std::string::size_type end = s.find(delimiter, start);

	while (end != std::string::npos) {
		if (end != start)
			result.push_back(s.substr(start, end - start));
		start = end + 1;
		end = s.find(delimiter, start);
	}
	if (start != s.size())
		result.push_back(s.substr(start));
	return result;
}

std::vector<std::string>	splitWhiteSpace(const std::string& s) {
	std::vector<std::string>	result;
	std::string::const_iterator start = std::find_if_not(s.begin(), s.end(),
														 std::isspace);
	std::string::const_iterator end = std::find_if(start, s.end(),
												   std::isspace);

	while (start != end) {
		result.push_back(std::string(start, end));
		start = std::find_if_not(end, s.end(), std::isspace);
		end = std::find_if(start, s.end(), std::isspace);
	}
	return result;
}
