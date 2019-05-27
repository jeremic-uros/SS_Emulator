
#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_
#include <unordered_map>
#include <string>
#include "ParsedLine.h"

namespace codes {

	std::unordered_map<std::string, unsigned char> instructionCodes = {
			{"halt", 1},
			{"xchg", 2},
			{"int" , 3},
			{"mov" , 4},
			{"add" , 5},
			{"sub" , 6},
			{"mul" , 7},
			{"div" , 8},
			{"cmp" , 9},
			{"not" , 10},
			{"and" , 11},
			{"or"  , 12},
			{"xor" , 13},
			{"test", 14},
			{"shl" , 15},
			{"shr" , 16},
			{"push", 17},
			{"pop" , 18},
			{"jmp" , 19},
			{"jeq" , 20},
			{"jne" , 21},
			{"jgt" , 22},
			{"call", 23},
			{"ret" , 24},
			{"iret", 25},
	};

	// possiblly refactoring needed
	std::unordered_map<std::string, unsigned char> addressingCodes = {
		{"imm"   ,    0},
		{"regdir",    1},
		{"regin",    2},
		{"reginx8",  3},
		{"reginx16", 4},
		{"mem",		  5}
	};			

	std::unordered_map<std::string, unsigned char> numOfOperands = {
			{"halt", 0},
			{"xchg", 2},
			{"int" , 1},
			{"mov" , 2},
			{"add" , 2},
			{"sub" , 2},
			{"mul" , 2},
			{"div" , 2},
			{"cmp" , 2},
			{"not" , 1},
			{"and" , 2},
			{"or"  , 2},
			{"xor" , 2},
			{"test", 2},
			{"shl" , 2},
			{"shr" , 2},
			{"push", 1},
			{"pop" , 1},
			{"jmp" , 1},
			{"jeq" , 1},
			{"jne" , 1},
			{"jgt" , 1},
			{"call", 1},
			{"ret" , 0},
			{"iret", 0},
	};
}

class Instruction : public ParsedLine {
private:
	std::string name;	 // instrucion symbol 
	unsigned char operandAttributes=0;	// bit values: ( 0 - operandSize[0 = one byte , 1 = two bytes] , 1 - low or high bits for first opr, 2 - low or high for second opr )
	std::string firstOprField="";				//
	unsigned char firstOprAddr=0;				//  only fields needed are filed, rest are undefined
	std::string secondOprField="";				//
	unsigned char secondOprAddr=0;				//
public:
	Instruction() : ParsedLine() {}
	Instruction(unsigned char typ, std::string lbl, unsigned char sz = 0) : ParsedLine(typ,lbl,sz) {}
	~Instruction() {}

	std::string getName() const { return name; }
	void setName(std::string nm) { name = nm; }

	unsigned char getOperandAttributes() const { return operandAttributes; }
	void setOperandAttributes(unsigned char oprAttr) { operandAttributes = oprAttr; }

	std::string getFirstOprField() const { return firstOprField; }
	void setFirstOprField(std::string dVal) { firstOprField = dVal; }

	unsigned char getFirstOprAddr() const { return firstOprAddr; }
	void setFirstOprAddr(unsigned char dAddr) { firstOprAddr = dAddr; }

	std::string getSecondOprField() const { return secondOprField; }
	void setSecondOprField(std::string sVal) { secondOprField = sVal; }

	unsigned char getSecondOprAddr() const { return secondOprAddr; }
	void  setSecondOprAddr(unsigned char sAddr) { secondOprAddr = sAddr; }


};

#endif // !_INSTRUCTION_H_
