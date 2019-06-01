#include "Section.h"
#include <iomanip>

void Section::addRelocationEntry(unsigned short offset, RelocationEntry::Type type, unsigned short symbol){
	if (!relocationTable) relocationTable = new std::list<RelocationEntry>;
	relocationTable->push_back(RelocationEntry(offset, type, symbol));
}

std::ostream & operator<<(std::ostream & it, const Section & sec){
	it << std::setw(15) << sec.name << std::setw(10) << sec.size << std::setw(10) << sec.rb;
	return it;
}
