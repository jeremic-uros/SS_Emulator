#ifndef _LINKER_H_
#define _LINKER_H_
#include "Symbol.h"
#include "Section.h"
#include <unordered_map>
#include <string>
#include <list>
#include <cstdint>
#include <iostream>

class Linker {
private:
	uint16_t symbolCounter;
	std::list<std::string> fileNames;
	std::string outputFile;

	std::unordered_map<std::string, Symbol> symbolTable;
	std::unordered_map<std::string,Section> sectionTable;
	std::unordered_map<uint16_t, Symbol> currentSymbolTable; // symbol table of the currently proccessed file;
	std::unordered_map<std::string,Section> currentSectionTable;
	std::unordered_map<std::string, uint16_t> sectionLocationCounterTable;
	uint8_t* sectionData;
	// only one global relocation table with run time addrs
	//std::list<RelocationEntry> relocationTable; 
	
	// Collects symbols and sections from all files 
	void firstPass();
	void secondPass();

	void loadSectionAndSymbolTable(std::ifstream& in);
	void addSections(); // adds sections if they dont exist otherwise updates section size
	void addSymbols(); // adds and updates values symbols from currentSymbolTable to symbolTable
	void updateSectionLocationCounters(); // adds the size of current file section to the counter

	void loadSectionData(std::ifstream & in);
	void loadRetTablesAndFix(std::ifstream& in);

	void output();
public:
	Linker(std::list<std::string> files, std::string output) : fileNames(files), outputFile(output), symbolCounter(1) {}

	void link();


};
#endif // !_LINKER_H_

