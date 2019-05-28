#include "Util.h"
#include <sstream>
#include <iomanip>

int util::convertStringToDecimal(std::string hexVal)
{
	std::stringstream ss;
	int decimal = 0;
	ss << hexVal;
	ss >> std::hex >> decimal;
	return decimal;
}
