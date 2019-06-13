#include "ObjectFileReader.h"
#include "Util.h"
#include <regex>
#include <iostream>
#include <fstream>

void ObjectFileReader::readSymbolandSectionTable(std::unordered_map<unsigned short, Symbol>& symTable, std::unordered_map<std::string, Section>& sectTable, std::ifstream & in){

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
			sectTable.insert({ name ,Section(name, size, rb) });
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
			Symbol::Type typeS = Symbol::Type::ABSGLOBAL;
			if (type == "GLOBAL")  typeS = Symbol::Type::GLOBAL;
			else  if (type == "LOCAL")typeS = Symbol::Type::LOCAL;
			uint16_t rb = util::convertStringToDecimal(tokens.front());
			symTable.insert({ rb, Symbol(name,section,val,typeS,rb) });
		}
	}
	else throw std::runtime_error("ERROR READING FROM FILE: BAD FILE FORMAT");
}

void ObjectFileReader::readRetTables(std::unordered_map<std::string, Section>& sectTable, std::ifstream & in){
	std::string line;
	while(std::getline(in,line) && !std::regex_search(line,std::regex("#\\.ret.+"))){}

	while (1) {
		if (!std::regex_search(line, std::regex("#\\.ret.+"))) break; // end of ret tables 
		std::string sectName = line.substr(5, sectName.npos);
		Section& sect = sectTable.at(sectName);
		while (std::getline(in, line) && std::regex_search(line, std::regex(".*[0-9]+.*(R_386_32|R_386_N32|R_386_PC32).*[0-9]+.*"))) {
			std::queue<std::string> tokens = util::tokenize(line, " ");
			// get RetEntry data from line
			uint16_t offset = util::convertStringToDecimal(tokens.front());
			tokens.pop();
			RelocationEntry::Type type;
			std::string typ = tokens.front();
			if (typ == "R_386_32") type = RelocationEntry::Type::R_386_32;
			else if (typ == "R_386_PC32") type = RelocationEntry::Type::R_386_PC32;
			else type = RelocationEntry::Type::R_386_N32;
			tokens.pop();
			uint16_t symReff = util::convertStringToDecimal(tokens.front());
			sect.addRelocationEntry(offset, type, symReff);
		}
	}

}
