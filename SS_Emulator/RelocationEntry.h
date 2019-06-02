#ifndef  _RELOCATION_ENTRY_H_
#define _RELOCATION_ENTRY_H_
#include <string>

class RelocationEntry{
public:
	enum Type {
		R_386_32 = 0,
		R_386_PC32 = 1,
		R_386_N32 = 2
	};
private:
	unsigned short offset;
	Type type;
	unsigned short symbol;
	// flags 
public:
	RelocationEntry (unsigned short offs, Type typ, unsigned short sym) : offset(offs), type(typ), symbol(sym) {}

	unsigned short getOffset() const { return offset; }
	void setOffset(unsigned short offs) { offset = offs; }

	unsigned short getType() const { return type; }
	void setType(Type typ) { type = typ; }

	unsigned short getSymbol() const { return symbol; }
	void setSymbol(unsigned short sym) { symbol = sym; }

protected:
	friend std::ostream& operator<< (std::ostream& it, const RelocationEntry& rel);

};
#endif // ! _RELOCATION_ENTRY_H_
