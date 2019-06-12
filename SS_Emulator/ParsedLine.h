#ifndef _PARSED_LINE_H_
#define _PARSED_LINE_H_

#include <string>

class ParsedLine{

private:
	unsigned char type;	// 0 - Instruction 1 - Directive 2 - Invalid line
	std::string label; // label name
	unsigned short size;  // mem size in bytes this line needs 
	std::string name;	// directive or instruction name	
protected:
	static const char delimiter = '!';
	virtual void write(std::ostream& it) const;
	friend std::ostream& operator<< (std::ostream& it, const ParsedLine& pe);

public:
	ParsedLine() {}
	ParsedLine(unsigned char typ, std::string lbl, std::string nam = "" , unsigned short sz = 0) : type(typ), label(lbl),name(nam), size(sz) {}
	unsigned char getType() const { return type; }
	std::string getLabel() const { return label; }
	unsigned short getSize() const { return size; }
	void setSize(unsigned short sz) { size = sz; }
	std::string getName() const { return name; }
	void setName(std::string nam) { name = nam; }

	// restores the previoulsy saved ParsedLine with operator<<
	virtual void restore(std::string);

	enum Types {
		Inst = 0,
		Dir = 1,
		Inv = 2,
		Label = 3
	};
};
#endif // _PARSED_LINE_H_
