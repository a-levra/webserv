#include "../include/utils/utils.hpp"

std::string getDate() {
	time_t now = time(NULL);
	tm *ltm = localtime(&now);
	std::string year = toString(1900 + ltm->tm_year);
	std::string month = toString(1 + ltm->tm_mon);
	std::string day = toString(ltm->tm_mday);
	std::string hour = toString(ltm->tm_hour);
	std::string min = toString(ltm->tm_min);
	std::string sec = toString(ltm->tm_sec);
	return (year + "-" + month + "-" + day + " " + hour + ":" + min + ":" + sec);
}