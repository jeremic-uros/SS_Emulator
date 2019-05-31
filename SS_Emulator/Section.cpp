#include "Section.h"

void Section::addRelocationEntry(unsigned short offset, RelocationEntry::Type type, unsigned short symbol){
	if (!relocationTable) relocationTable = new std::list<RelocationEntry>;
	relocationTable->push_back(RelocationEntry(offset, type, symbol));
}
