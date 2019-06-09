#include "Linker.h"
#include "Util.h"
#include "Section.h"
#include "RelocationEntry.h"
#include "Symbol.h"
#include "ObjectFileReader.h"
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
	std::ifstream in;

	// set section Location Counters to section start values
	for (auto& cnt : sectionLocationCounterTable)
		cnt.second = symbolTable.at(cnt.first).value;


	// allocate data array
	uint16_t size = 0;
	for (auto sect : sectionTable) {
		if (sect.first != ".bss") {
			size += sect.second.size;
		}
	}
	sectionData = new uint8_t[size];

	for (int i = 0; i < numOfFiles; i++) {
		in.open(fileNames[i]);
		loadSectionAndSymbolTable(in);
		loadSectionData(in);
		loadRetTablesAndFix(in);
		updateSectionLocationCounters();
		in.close();
	}
}

void Linker::loadSectionAndSymbolTable(std::ifstream & in){
	// clear current section and symbol table
	currentSectionTable.clear();
	currentSymbolTable.clear();

	// load section and symbol table
	ObjectFileReader::readSymbolandSectionTable(currentSymbolTable, currentSectionTable, in);
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


void Linker::loadSectionData(std::ifstream & in){
	// file position should be at the begining of section data 
	// read section data
	std::string line;
	uint16_t locCounter = 0;
	while (std::getline(in, line) && std::regex_search(line, std::regex("#\\.[a-z]+"))) {
		std::string sectName = line.substr(1, line.npos);
		if (sectName != ".bss") {
			uint16_t size = currentSectionTable.at(sectName).size;
			uint8_t offset = sectionLocationCounterTable.at(sectName);
			in.read((char*)(sectionData + offset), size);
		}
	}
}

void Linker::loadRetTablesAndFix(std::ifstream & in) {
	std::string line;
	std::getline(in, line); // line should be a #.rel table

	// read ret tables
	while (1) {
		if(!std::regex_search(line,std::regex("#\\.ret.+"))) break; // end of ret tables 
		std::string sectName = line.substr(5, sectName.npos);
		Section& sect = sectionTable.at(sectName);

		while (std::getline(in, line) && std::regex_search(line, std::regex(".*[0-9]+.*(R_386_32|R_386_N32|R_386_PC32).*[0-9]+.*"))) {
			std::queue<std::string> tokens = util::tokenize(line, " ");
			// get RetEntry data from line
			uint16_t oldOffset = util::convertStringToDecimal(tokens.front());
			tokens.pop();
			RelocationEntry::Type type;
			std::string typ = tokens.front();
			if (typ == "R_386_32") type = RelocationEntry::Type::R_386_32;
			else if (typ == "R_386_PC32") type = RelocationEntry::Type::R_386_PC32;
			else type = RelocationEntry::Type::R_386_N32;
			tokens.pop();
			uint16_t oldSymReff = util::convertStringToDecimal(tokens.front());

			// get reffrenced symbol from sym table
			std::string symName = currentSymbolTable.at(oldSymReff).name;
			Symbol& symbol = symbolTable.at(symName);
			uint16_t newOffset = oldOffset + sectionLocationCounterTable.at(sectName); // oldOffset + run time start address of the section

			// no need for reallocating if a undefined symbol is now defined in the same section and adr type is PC REl
			if (currentSymbolTable.at(oldSymReff).section != 0 || 
				symbol.section != sectionTable.at(sectName).rb ||
				type != RelocationEntry::Type::R_386_PC32){ 
				sect.addRelocationEntry(newOffset, type, symbol.section);
			}

			// fix data
			int16_t oldVal = ((uint16_t)sectionData[newOffset + 1]) << 8 | sectionData[newOffset];
			int16_t newVal = 0;
			if (symbol.rb == symbol.section) newVal = sectionLocationCounterTable.at(symbol.name);
			else newVal = symbol.value;
			switch (type) {
			case RelocationEntry::R_386_32:
				newVal = oldVal + newVal;
				break;
			case RelocationEntry::R_386_PC32:
				newVal = oldVal + newVal - newOffset;
				break;
			case RelocationEntry::R_386_N32:
				newVal = oldVal - newVal;
				break;
			default:
				break;
			}

			sectionData[newOffset] = newVal & 0xFF;
			sectionData[newOffset + 1] = newVal >> 8;
		}
	}
}

void Linker::output(){
	std::ofstream out(outputFile);
	
	// section table
	uint16_t size = 0;
	out << "#sectab" << std::endl;
	for (auto const& sec : sectionTable) {
		if (sec.first != ".bss") size += sec.second.size;
		out << sec.second << std::endl;
	}
	// symbol table
	out << "#symtab" << std::endl;
	for (auto const& sym : symbolTable) 
		if (sym.second.section == sym.second.rb) 
			out << sym.second << std::endl;

	out << std::endl;
	// data 
	out.write((char*)sectionData, size);

	out << std::endl;


	// ret tables
	for (auto const& sec : sectionTable) {
		if (sec.second.relocationTable) {
			out << "#.ret" + sec.second.name << std::endl;
			for (auto const& rel : *sec.second.relocationTable)
				out << rel << std::endl;
		}
	}

	out.close();

	for (int i = 0; i < size; i++) {
		std::cout << util::convertDecimalToString(sectionData[i], true) << " ";
	}
}


void Linker::link(){

	firstPass();

	secondPass();

	output();

	// free allocated space for section data
	delete sectionData;
}
