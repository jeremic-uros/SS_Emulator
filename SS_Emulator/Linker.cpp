#include "Linker.h"
#include "Util.h"
#include "Section.h"
#include "Symbol.h"
#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <queue>
#include <cstdint>

void Linker::firstPass(){
	std::ifstream in;

	// collect symbols from all files
	for (int i = 0; i < numOfFiles; i++) {
		in.open(fileNames[i]);
		loadSectionAndSymbolTable(in);
		in.close();
		addSections();
		addSymbols();
		updateSectionLocationCounters();
	}

	// check if there is undefined symbols or symbol main is not defined[TO:DO]
	

	// update section start values

	uint16_t locationCounter = 0;
	std::unordered_map<uint16_t, uint16_t> startAddrs; // temp map for section rb to section run time addr
	for (auto const& sect : sectionTable) {
		symbolTable.at(sect.first).value = locationCounter;
		startAddrs.insert({ sect.second.rb,locationCounter });
		locationCounter += sect.second.size;
	}

	// update symbol offset to run time addrs
	for (auto& sym : symbolTable) {
		if (sym.second.section != sym.second.rb) { // update only non section symbols
			sym.second.value += startAddrs.at(sym.second.section);
		}
	}
}

void Linker::secondPass(){
}

void Linker::loadSectionAndSymbolTable(std::ifstream & in){
	// clear current section and symbol table
	currentSectionTable.clear();
	currentSymbolTable.clear();

	// load section table
	std::string line;
	while (std::getline(in, line) && line != "#sectab") {}
	if (line == "#sectab") {
		while (std::getline(in, line) && std::regex_search(line, std::regex(".*\\.[a-z]+.*[0-9]+.*[0-9]+"))) {
			std::queue<std::string> tokens = util::tokenize(line, " ");
			std::string name = tokens.front();
			tokens.pop();
			uint16_t size = util::convertStringToDecimal(tokens.front());
			tokens.pop();
			uint8_t rb = util::convertStringToDecimal(tokens.front());
			currentSectionTable.insert({ name ,Section(name, size, rb) });
		}
	}
	else {} // throw bad file format exception TO:DO

	// load symbol table
	if (line == "#symtab") {
		while (std::getline(in, line) && std::regex_search(line, std::regex(".*[a-z]+.*[0-9]+.*[0-9]+.*(LOCAL|GLOBAL).*[0-9]+"))) {
			std::queue<std::string> tokens = util::tokenize(line, " ");
			std::string name = tokens.front();
			tokens.pop();
			uint16_t section = util::convertStringToDecimal(tokens.front());
			tokens.pop();
			uint16_t val = util::convertStringToDecimal(tokens.front());
			tokens.pop();
			std::string type = tokens.front();
			tokens.pop();
			uint16_t rb = util::convertStringToDecimal(tokens.front());
			currentSymbolTable.insert({ rb, Symbol(name,section,val,(type == "GLOBAL" ? Symbol::Type::GLOBAL : Symbol::Type::LOCAL),rb) });
		}
	}
	else {} // throw bad file format exception TO:DO

}

void Linker::addSections(){
	for (auto section : currentSectionTable) {
		if (sectionTable.find(section.first) == sectionTable.end()) { // not found
			section.second.rb = symbolCounter++; // new rb, will be added to sym table after using same rb
			sectionTable.insert({ section.first, section.second });
			sectionLocationCounterTable.insert({ section.first , 0 }); 
		}
		else { // found
			sectionTable.at(section.first).size += section.second.size;
		}
	}
}

void Linker::addSymbols(){
	for (auto symbol : currentSymbolTable) {
		if (symbolTable.find(symbol.second.name) == symbolTable.end()) { // not found
			if (symbol.second.rb == symbol.second.section) { // if section
				symbol.second.rb = sectionTable.at(symbol.second.name).rb; // update to new rb
				symbol.second.section = symbol.second.rb;
			}
			else {
				symbol.second.rb = symbolCounter++;
				if (symbol.second.section != 0) {
					std::string sectionName = currentSymbolTable.at(symbol.second.section).name; 
					symbol.second.section = sectionTable.at(sectionName).rb; // update section rb
					symbol.second.value += sectionLocationCounterTable.at(sectionName); // update offset in section
				}
			}
			symbolTable.insert({ symbol.second.name, symbol.second });
		}
		else { //	found
			Symbol& sym = symbolTable.at(symbol.second.name);
			if (sym.section == 0) { // if symbol was undefined 
				std::string sectionName = currentSymbolTable.at(symbol.second.section).name;
				sym.section = sectionTable.at(sectionName).rb; // update section rb
				sym.value = symbol.second.value + sectionLocationCounterTable.at(sectionName); // update offset in section
			}
			else {
				if (symbol.second.section == 0) continue; // udefined symbol already defined, skip ahead
				else {} // throw multiply definitions exception
			}
		}
	}
}

void Linker::updateSectionLocationCounters(){
	for (auto& section : currentSectionTable) {
		sectionLocationCounterTable.at(section.first) += section.second.size;
	}
}


void Linker::loadRelTables(std::ifstream & in){
}

void Linker::fixRelTablesReffrences()
{
}

void Linker::link(){

	firstPass();

}
