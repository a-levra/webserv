#include "utils/utils.hpp"

//this function returns a vector of all positions where the "word" has been found
//if the "word" does not occur at all in the string, an empty vector is returned

std::vector<size_t> getPositionsOfWord(const std::string &word, const std::string &src){
	std::vector<size_t> positions;
	size_t nextPos;
	nextPos = src.find(word);
	while (nextPos != std::string::npos){
		positions.push_back(nextPos);
		nextPos = src.find(word, nextPos);
	}
	return positions;
}