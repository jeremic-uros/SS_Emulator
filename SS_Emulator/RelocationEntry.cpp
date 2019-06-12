#include "RelocationEntry.h"
#include <iomanip>
#include <iostream>
#include <string>

std::ostream & operator<<(std::ostream & it, const RelocationEntry & rel){
	it << std::setw(10) << rel.offset << std::setw(10);
	std::string type = "R_386_32";
	if (rel.type == RelocationEntry::Type::R_386_PC32) type = "R_386_PC32";
	else if (rel.type == RelocationEntry::Type::R_386_N32) type = "R_386_N32";
	it << std::setw(12) << type << std::setw(10) << rel.symbol;
	return it;
}
