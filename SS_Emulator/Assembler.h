#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_
#include <string>
#include <unordered_map>
#include "Symbol.h"


class Assembler {
private:
	const std::string workFileName = "Assembler_work_file.txt";
	unsigned short symbolCounter = 1;
	std::string inputFileName;
	std::string outputFileName;
	std::unordered_map<std::string,Symbol> symbolTable;

	void firstRun();
	void secondRun();
	void reset(std::string in, std::string out);
	void addSymbol(std::string nm, std::string sec, unsigned short val, Symbol::Type typ, unsigned short rbb);

public:
	Assembler(std::string in, std::string out) : inputFileName(in), outputFileName(out) {}
	bool assemble();
};
#endif // !_ASSEMBLER_H_

