#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_
#include <string>
#include <unordered_map>
#include "Symbol.h"
#include "Section.h"

class Instruction;
class Directive;

class Assembler {
private:
	const std::string workFileNameFirstRun = "AssemblerWorkFile_firstRun.txt";
	const std::string workFileNameSecondRun = "AssemblerWorkFile_secondRun.txt";
	unsigned short symbolCounter = 1;
	unsigned short locationCounter = 0;
	std::string currentSection;
	std::string inputFileName;
	std::string outputFileName;
	std::unordered_map<std::string,Symbol> symbolTable;
	std::unordered_map<std::string,Section> sectionTable;

	void firstRun();
	void secondRun();
	void reset(std::string in, std::string out);
	void addSymbol(std::string nm, std::string sec, unsigned short val, Symbol::Type typ, unsigned short rbb);
	void addSection(std::string name, unsigned short size, unsigned short rb);
	signed short handleSymbol(std::string name, RelocationEntry::Type type);
	void handleInstruction(Instruction* inst);
	void handleDirective(Directive * dir, std::ofstream& out);

public:
	Assembler(std::string in, std::string out) : inputFileName(in), outputFileName(out) {}
	bool assemble();
};
#endif // !_ASSEMBLER_H_

