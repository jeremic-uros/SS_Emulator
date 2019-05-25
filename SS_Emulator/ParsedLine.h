#ifndef _PARSED_LINE_H_
#define _PARSED_LINE_H_

#include <string>

class ParsedLine{

protected:
	unsigned char type;	// 0 - Instruction 1 - Directive 2 - Invalid line
	std::string label; // label name
	unsigned char size;  // mem size in bytes this line needs 

public:
	ParsedLine() {}
	ParsedLine(unsigned char typ, std::string lbl, unsigned char sz = 0) : type(typ), label(lbl), size(sz) {}
	unsigned char getType() const { return type; }
	std::string getLabel() const { return label; }
	unsigned char getSize() const { return size; }
	void setSize(unsigned char sz) { size = sz; }

	enum Types {
		Inst = 0,
		Dir = 1,
		Inv = 2
	};
};
#endif // _PARSED_LINE_H_
