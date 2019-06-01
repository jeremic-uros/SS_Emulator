#include "Symbol.h"
#include <iomanip>

std::ostream & operator<<(std::ostream & it, const Symbol & sym) {
	it << std::setw(15) << sym.name << std::setw(10) << sym.section << std::setw(10) << sym.value;
	std::string type = "LOCAL";
	if (sym.type == sym.ABSOLUT) type = "ABSOLUT";
	else if (sym.type == sym.GLOBAL)  type = "GLOBAL";
	it << std::setw(10) << type << std::setw(10) << sym.rb;
	return it;
}
