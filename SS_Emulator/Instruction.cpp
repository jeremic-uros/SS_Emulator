#include "Instruction.h"
#include "Util.h"
#include <queue>
#include <unordered_map>
#include <string>
#include <iostream>

std::unordered_map<std::string, unsigned char> Instruction::instructionCodes = {
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

std::unordered_map<std::string, unsigned char> Instruction::addressingCodes = {
	{"imm"   ,    0},
	{"regdir",    1},
	{"regin",    2},
	{"reginx8",  3},
	{"reginx16", 4},
	{"mem",		  5}
};

std::unordered_map<std::string, unsigned char> Instruction::numOfOperands = {
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

std::unordered_map<unsigned short, std::string> Instruction::instructionNames = {
		{ 1, "halt"},
		{ 2, "xchg"},
		{ 3, "int"},
		{ 4, "mov"},
		{ 5, "add"},
		{ 6, "sub"},
		{ 7, "mul"},
		{ 8, "div"},
		{ 9, "cmp"},
		{ 10 , "not"},
		{ 11 , "and"},
		{ 12 , "or"},
		{ 13 , "xor"},
		{ 14 , "test"},
		{ 15 , "shl"},
		{ 16 , "shr"},
		{ 17 , "push"},
		{ 18 , "pop"},
		{ 19 , "jmp"},
		{ 20 , "jeq"},
		{ 21 , "jne"},
		{ 22 , "jgt"},
		{ 23 , "call"},
		{ 24 , "ret"},
		{ 25 , "iret"},
};
void Instruction::write(std::ostream & it) const {
	ParsedLine::write(it);
	it << (int) operandAttributes << (std::string) "!" << "fOpr=" + firstOprField << (std::string) "!" << (int)firstOprAddr; 		   	it <<  (std::string) "!" << "sOpr=" + secondOprField << (std::string) "!" << (int)secondOprAddr <<  (std::string) "!";
}

void Instruction::restore(std::string line) {
	std::queue<std::string> tokens = util::tokenize(line, "!");
	std::string lineForBaseClass = "";
	for (int i = 0; i < 4; i++) {
		lineForBaseClass += tokens.front() + "!";
		tokens.pop();
	}
	ParsedLine::restore(lineForBaseClass);
	// get OperandAttr
	operandAttributes = util::convertStringToDecimal(tokens.front());
	tokens.pop();
	// get First Opr field
	firstOprField = tokens.front();
	tokens.pop();
	if (firstOprField == "fOpr=") firstOprField = "";
	else firstOprField = firstOprField.substr(5, firstOprField.size() - 1);
	// get First Opr addr
	firstOprAddr = util::convertStringToDecimal(tokens.front());
	tokens.pop();
	// get Second Opr field
	secondOprField = tokens.front();
	tokens.pop();
	if (secondOprField == "sOpr=") secondOprField = "";
	else secondOprField = secondOprField.substr(5, secondOprField.size() - 1);
	// get Second Opr Addr
	secondOprAddr = util::convertStringToDecimal(tokens.front());
}
