#ifndef  _SECTION_H_
#define _SECTION_H_
#include "RelocationEntry.h"
#include <string>
#include <list>

class Section {
public:
	std::string name;
	unsigned short size;
	unsigned short rb;
	std::list<RelocationEntry>* relocationTable = nullptr;

	Section(std::string nam, unsigned short sz,unsigned short rbb ) : name(nam), size(sz), rb(rbb) {}
	~Section() { if (relocationTable) free(relocationTable); }

	std::string getName() const { return name; }
	void setName(std::string nam) { name = nam; }

	unsigned short getSize() const { return size; }
	void setSize(unsigned short sz) { size = sz; }

	unsigned short getRb() const { return rb; }

	void addRelocationEntry(unsigned short offset, RelocationEntry::Type type, unsigned short symbol);
protected:
	friend std::ostream& operator<< (std::ostream& it, const Section& sym);

};
#endif // ! _SECTION_H_
