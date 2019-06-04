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
	uint16_t symbolCounter = 0;

	std::unordered_map<std::string, Symbol> symbolTable;
	std::list<Section> sectionTable;
	std::unordered_map<uint16_t, Symbol> currentSymbolTable; // symbol table of the currently proccessed file;
	std::list<Section> currentSectionTable;
	std::unordered_map<std::string, uint16_t> sectionLocationCounterTable;
	std::unordered_map<std::string, std::list<RelocationEntry>> relocationTable;

public:
	// Collects symbols and sections from all files 
	void firstPass();
	void secondPass();

	void loadSectionAndSymbolTable(std::ifstream& in);
	void loadRelTables(std::ifstream& in);
	void fixRelTablesReffrences();

};
#endif // !_LINKER_H_

