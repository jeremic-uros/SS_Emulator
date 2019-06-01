#include "Symbol.h"

std::ostream & operator<<(std::ostream & it, const Symbol & sym) {
	it << sym.name << " " << sym.section << " " << sym.value << " ";
	std::string type = "LOCAL";
	if (sym.type == sym.ABSOLUT) type = "ABSOLUT";
	else if (sym.type == sym.GLOBAL)  type = "GLOBAL";
	it << type << " " << sym.rb;
	return it;
}
