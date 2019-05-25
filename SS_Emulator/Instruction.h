
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
		{"regind",    2},
		{"regindx8",  3},
		{"regindx16", 4},
		{"mem",		  5}
	};			

}

class Instruction : ParsedLine {
private:
	std::string name;	 // instrucion symbol 
	unsigned char operandAttributes;	// bit values: ( 0 - operandSize[0 = one byte , 1 = two bytes] , 1 - low or high bits )
	unsigned short dstField;        //
	unsigned char dstAddr;			//  only fields needed are filed, rest are undefined
	unsigned short srcField;		//
	unsigned char srcAddr;			//
public:
	Instruction() : ParsedLine() {}

	std::string getName() const { return name; }
	Instruction& setName(std::string nm) { name = nm; }

	unsigned char getOperandAttributes() const { return operandAttributes; }
	Instruction& setOperandAttributes(unsigned char oprAttr) { operandAttributes = oprAttr; }

	unsigned short getDstField() const { return dstField; }
	Instruction& setDstField(unsigned short dVal) { dstField = dVal; }

	unsigned char getDstAddr() const { return dstAddr; }
	Instruction& setDstAddr(unsigned char dAddr) { dstAddr = dAddr; }

	unsigned short getSrcField() const { return srcField; }
	Instruction& setSrcField(unsigned short sVal) { srcField = sVal; }

	unsigned char getSrcAddr() const { return srcAddr; }
	Instruction&  setSrcAddr(unsigned char sAddr) { srcAddr = sAddr; }


};

#endif // !_INSTRUCTION_H_
