#ifndef _UTIL_H_
#define _UTIL_H_
#include <string>
#include <queue>

namespace util {

	int convertStringToDecimal(std::string); // converts hex or decimal string to int
	std::string convertDecimalToString(signed short decimal, bool truncateToByte); // converts short to hex string without 0x prefix
	std::queue<std::string> tokenize(std::string input, std::string delimiter);

	/*template <typename T>
	void writeTable(std::unordered_map<std::string,T>,std::ostream& out);
	template<typename T>
	void writeTable(std::unordered_map<std::string, T>, std::ostream & out){

	}*/
}

#endif // !_UTIL_H_
