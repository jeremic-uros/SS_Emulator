#include "Util.h"
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <regex>
#include <string>
#include <iostream>

int util::convertStringToDecimal(std::string val){
	std::stringstream ss;
	int decimal = 0;
	if (val[1] == 'x' || (val.size() >= 3 && val[2] == 'x')) {
		ss << val;
		ss >> std::hex >> decimal;
	}
	else decimal = stoi(val);
	return decimal;
}

std::string util::convertDecimalToString(signed short decimal,bool truncateToByte){
	std::stringstream ss;
	ss << std::setfill('0') << std::setw(4) <<std::hex << decimal;
	std::string ret = ss.str();
	int sz = ret.size();
	if (truncateToByte) return ret.substr(2, 2 );
	return ret.substr(2, 2) + " " + ret.substr(0, 2);
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

