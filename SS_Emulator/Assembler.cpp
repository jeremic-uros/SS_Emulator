#include "Assembler.h"
#include "Lexer.h"
#include "AssemblerException.h"
#include "Parser.h"
#include "Instruction.h"
#include "Directive.h"
#include "Util.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <regex>

void Assembler::firstRun(){
	// Prepare files and tools
	bool assembling = true;
	std::ofstream out;
	std::ifstream in;
	in.open(inputFileName);
	out.open(workFileName);
	std::string line;
	std::string currentSection = "";
	unsigned int locationCounter = 0;
	std::regex sectionRegex("^(data|text|bss|section)$");

	Lexer* lexer = Lexer::getLexer();
	Parser* parser = Parser::getParser();


	// loop until end of file or .end directive reached
	while (assembling && std::getline(in, line)) {
		// get ParsedLine from line
		ParsedLine* parsedLine = parser->parse(lexer->tokenize(line));

		// write to workFile for second run, empty lines and .equ directive are not writen
		if(parsedLine && parsedLine->getName() != "equ") out << (*parsedLine) << std::endl;
		// proccess parsedLine object
		if (parsedLine) {
			std::string name = parsedLine->getName();
			std::string label = parsedLine->getLabel();
			unsigned char size = parsedLine->getSize();
			if (name == "end") assembling = false;
			else if (std::regex_search(name,sectionRegex)) { 
				// reset location counter and change current section
				locationCounter = 0;
				if (name == "section") currentSection = ((Directive*)parsedLine)->getParam();
				else currentSection = "."+ name;
				// add symbol
				addSymbol(currentSection, currentSection, locationCounter, Symbol::Type::LOCAL, symbolCounter++);
			}
			else if (name == "equ") {
				std::string param = ((Directive*)parsedLine)->getParam();
				std::queue<std::string> paramTokens = util::tokenize(param, ",");
				if (label != "") addSymbol(label, currentSection, locationCounter, Symbol::Type::LOCAL, symbolCounter++);
				std::string symName = paramTokens.front();
				paramTokens.pop();
				unsigned short symVal = util::convertStringToDecimal(paramTokens.front());
				addSymbol(symName, currentSection, symVal, Symbol::Type::ABSOLUT, symbolCounter++);
			}
			else {
				// If line is labled add a new symbol
				if (label != "") addSymbol(label, currentSection, locationCounter, Symbol::Type::LOCAL, symbolCounter++);
				// Update locationCounter
				if (name == "align") {
					unsigned short alignParam = util::convertStringToDecimal(((Directive*)parsedLine)->getParam());
					size = alignParam - (locationCounter % alignParam);
				}
				locationCounter += size;
			}
		}
	}


	out.close();
	in.close();

	// no .end directive found throw error
	if (assembling == true) throw util::AssemblerException("ASSEMBLING ERROR: NO .end directive found");
}

void Assembler::secondRun(){

}

void Assembler::reset(std::string in, std::string out){
	inputFileName = in;
	outputFileName = out;
	symbolTable.clear();
	symbolCounter = 1;
}

void Assembler::addSymbol(std::string name, std::string section, unsigned short val, Symbol::Type type, unsigned short rb){
	if (symbolTable.find(name) == symbolTable.end()) {
		if (section == "") throw util::AssemblerException("ASSEMBLING ERROR: Symbol "+name+"not defined in a section");
		Symbol newSymbol(name, section, val, type, rb);
		symbolTable.insert({ name, newSymbol });
	}
	else throw util::AssemblerException("ASSEMBLING ERROR: Symbol " + name + " defined more than once");
}

bool Assembler::assemble(){
	try {
		Symbol und("UND", "UND", 0, Symbol::Type::LOCAL, 0);
		std::string undefined = "UND";
		symbolTable.insert({ undefined,und });
		
		firstRun();

	}
	catch (util::AssemblerException e) {
		std::cout << e << std::endl;
	}
	return false;
}
