
#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_
#include <unordered_map>
#include <string>
#include "ParsedLine.h"


class Instruction : public ParsedLine {
private:
	/* 
		bit values: 
			0 - operandSize[0 = one byte , 1 = two bytes]
			1 - low or high bits for first opr
			2 - low or high for second opr 
			3 - is pcrel [0 - false , 1 - true] , for creating relocation entries
	*/
	unsigned char operandAttributes;	
	std::string firstOprField;				//
	unsigned char firstOprAddr;				//  only fields needed are filed, rest are undefined
	std::string secondOprField;				//
	unsigned char secondOprAddr;				//
protected:
	void write(std::ostream& it) const override;
public:
	Instruction() : ParsedLine() {}
	Instruction(unsigned char typ, std::string lbl, std::string nam="",unsigned char sz = 0) : ParsedLine(typ,lbl,nam,sz),firstOprField(""),firstOprAddr(0),secondOprField(""),secondOprAddr(0) {}
	~Instruction() {}

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

	void restore(std::string line) override;

	static std::unordered_map<std::string, unsigned char> instructionCodes;

	

	enum InstrCodes {
		HALT = 1,
		XCHG = 2,
		INT = 3,
		MOV = 4,
		ADD = 5,
		SUB = 6,
		MUL = 7,
		DIV = 8,
		CMP = 9,
		NOT = 10,
		AND = 11,
		OR = 12,
		XOR = 13,
		TEST = 14,
		SHL = 15,
		SHR = 16,
		PUSH = 17,
		POP = 18,
		JMP = 19,
		JEQ = 20,
		JNE = 21,
		JGT = 22,
		CALL = 23,
		RET = 24,
		IRET = 25,
	};

	static std::unordered_map<unsigned short, std::string> instructionNames;
	// possiblly refactoring needed
	static std::unordered_map<std::string, unsigned char> addressingCodes;

	enum AddrCodes {
		IMM = 0,
		REGDIR = 1,
		REGIN = 2,
		REGINX8 = 3,
		REGINX16 = 4,
		MEM = 5
	};

	static std::unordered_map<std::string, unsigned char> numOfOperands;
};

#endif // !_INSTRUCTION_H_
