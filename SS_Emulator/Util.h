#ifndef _UTIL_H_
#define _UTIL_H_
#include <string>
#include <queue>

namespace util {

	int convertStringToDecimal(std::string); // converts hex or decimal string to int
	std::queue<std::string> tokenize(std::string input, std::string delimiter);
}

#endif // !_UTIL_H_
