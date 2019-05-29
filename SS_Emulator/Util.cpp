#include "Util.h"
#include <sstream>
#include <iomanip>
#include <regex>

int util::convertStringToDecimal(std::string hexVal){
	std::stringstream ss;
	int decimal = 0;
	ss << hexVal;
	ss >> std::hex >> decimal;
	return decimal;
}

std::queue<std::string> util::tokenize(std::string input, std::string delimiter){
	std::smatch match;
	std::regex regex("[^"+delimiter+"]+");
	std::queue<std::string> tokens;

	while (std::regex_search(input, match, regex)) {
		tokens.push(match.str(0));
		input = match.suffix().str();
	}
	return tokens;
}
