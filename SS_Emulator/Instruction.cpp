#include "Instruction.h"
#include "Util.h"
#include <queue>

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

void Instruction::write(std::ostream & it) const {
	ParsedLine::write(it);
	it << (int)operandAttributes << delimiter << "fOpr=" + firstOprField << delimiter << (int)firstOprAddr << delimiter << "sOpr=" + secondOprField << delimiter << (int)secondOprAddr << delimiter;
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
