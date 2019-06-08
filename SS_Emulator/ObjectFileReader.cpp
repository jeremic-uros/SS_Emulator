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
			uint16_t rb = util::convertStringToDecimal(tokens.front());
			symTable.insert({ rb, Symbol(name,section,val,(type == "GLOBAL" ? Symbol::Type::GLOBAL : Symbol::Type::LOCAL),rb) });
		}
	}
	else throw std::runtime_error("ERROR READING FROM FILE: BAD FILE FORMAT");
}
