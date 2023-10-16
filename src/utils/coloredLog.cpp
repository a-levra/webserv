#include "../include/utils/utils.hpp"

void coloredLog(const std::string& str1, const std::string& str2, colors color)
{
	std::string colorCode;
	switch (color)
	{
		case RED:
			colorCode = "\033[1;31m";
			break;
		case GREEN:
			colorCode = "\033[1;32m";
			break;
		case YELLOW:
			colorCode = "\033[1;33m";
			break;
		case BLUE:
			colorCode = "\033[1;34m";
			break;
		case GREY:
			colorCode = "\033[1;30m";
			break;
		case PURPLE:
			colorCode = "\033[1;35m";
			break;
		default:
			colorCode = "\033[1;37m";
			break;
	}
	printBoldThenThin(colorCode + str1, str2);
}