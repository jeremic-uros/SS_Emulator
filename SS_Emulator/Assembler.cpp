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
	out.open(workFileNameFirstRun);
	std::string line;
	currentSection = "";
	locationCounter = 0;
	std::regex sectionRegex("^(data|text|bss|section)$");

	Lexer* lexer = Lexer::getLexer();
	Parser* parser = Parser::getParser();


	// loop until end of file or .end directive reached
	while (assembling && std::getline(in, line)) {
		// get ParsedLine from line
		ParsedLine* parsedLine = parser->parse(lexer->tokenize(line));

		// proccess parsedLine object
		if (parsedLine) {
			std::string name = parsedLine->getName();
			std::string label = parsedLine->getLabel();
			unsigned char size = parsedLine->getSize();
			if (name == "end") assembling = false;
			else if (std::regex_search(name,sectionRegex)) { 
				// reset location counter and change current section
				if (currentSection != "") sectionTable.at(currentSection).setSize(locationCounter);
				locationCounter = 0;
				if (name == "section") currentSection = ((Directive*)parsedLine)->getParam();
				else currentSection = "."+ name;
				addSection(currentSection, 0, symbolCounter);
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
			else if(name == "extern"){
				std::string param = ((Directive*)parsedLine)->getParam();
				std::queue<std::string> paramTokens = util::tokenize(param, ",");
				while (!paramTokens.empty()) {
					std::string symName = paramTokens.front();
					paramTokens.pop();
					addSymbol(symName, "UND", 0, Symbol::Type::LOCAL, symbolCounter++);
				}
			}
			else {
				// If line is labled add a new symbol
				if (label != "") addSymbol(label, currentSection, locationCounter, Symbol::Type::LOCAL, symbolCounter++);
				// Update locationCounter
				if (name == "align") {
					unsigned short alignParam = util::convertStringToDecimal(((Directive*)parsedLine)->getParam());
					if (alignParam != 0) {
						size = alignParam - (locationCounter % alignParam);
						parsedLine->setSize(size);
					}
				}
				else if ( name == "word" && locationCounter & 1 )  parsedLine->setSize(3);
				locationCounter += size;
			}

			
		}
		else continue;

		// write to workFile for second run, empty lines and .equ directive are not writen
		if (parsedLine && parsedLine->getName() != "equ") out << (*parsedLine) << std::endl;
		free(parsedLine);

	}

	// update last section size
	sectionTable.at(currentSection).setSize(locationCounter);


	out.close();
	in.close();

	// no .end directive found throw error
	if (assembling == true) throw util::AssemblerException("ASSEMBLING ERROR: NO .end directive found");
}

void Assembler::secondRun() {
	// Prepare files and tools
	bool assembling = true;
	std::ofstream out;
	std::ifstream in;
	in.open(workFileNameFirstRun);
	out.open(workFileNameSecondRun);
	std::string line;
	currentSection = "";
	locationCounter = 0;


	// loop until end of file or .end directive reached
	while (assembling && std::getline(in, line)) {
		// Restore saved line
			// get type from line (im lazy to rewrite)
		ParsedLine* parsedLine = nullptr;
		short type = line[0] - '0';
		switch (type){
		case ParsedLine::Types::Inst:
			parsedLine = new Instruction();
			break;
		case ParsedLine::Types::Dir:
			parsedLine = new Directive();
			break;
		default:
			parsedLine = new ParsedLine();
			break;
		}
		parsedLine->restore(line);

		std::string name = parsedLine->getName();
		unsigned char size = parsedLine->getSize();

		if (name == "end") { assembling = false; }
		// Directive handling
		if (type == ParsedLine::Types::Dir) handleDirective((Directive*)parsedLine,out);
		else if(type == ParsedLine::Types::Inst) {
			locationCounter += parsedLine->getSize();
		}
		// free allocated memory
		free(parsedLine);
	}

	out.close();
	in.close();

	// no .end directive found throw error
	if (assembling == true) throw util::AssemblerException("ASSEMBLING ERROR: NO .end directive found");
}

void Assembler::reset(std::string in, std::string out){
	inputFileName = in;
	outputFileName = out;
	symbolTable.clear();
	symbolCounter = 1;
}

void Assembler::addSymbol(std::string name, std::string section, unsigned short val, Symbol::Type type, unsigned short rb){
	if (symbolTable.find(name) == symbolTable.end()) {
		if (section == "") throw util::AssemblerException("ASSEMBLING ERROR: Symbol "+name+" not defined in a section");
		unsigned short sec;
		if(section == "UND") sec = 0;
		else sec = sectionTable.at(section).getRb();
		Symbol newSymbol(name, sec , val, type, rb);
		symbolTable.insert({ name, newSymbol });
	}
	else throw util::AssemblerException("ASSEMBLING ERROR: Symbol " + name + " defined more than once");
}

void Assembler::addSection(std::string name, unsigned short size, unsigned short rb){
	if (sectionTable.find(name) == sectionTable.end()) {
		Section section(name, size, rb);
		sectionTable.insert({ name,section });
	}
}

signed short Assembler::handleSymbol(std::string name, RelocationEntry::Type type){
	Symbol& sym = symbolTable.at(name);
	if (sym.type == Symbol::Type::ABSOLUT) return sym.value;
	Section& sect = sectionTable.at(currentSection);
	if (type == RelocationEntry::Type::R_386_32) {
		if (sym.type == Symbol::Type::GLOBAL) {
			sect.addRelocationEntry(locationCounter, type, sym.rb);
			return 0;
		}
		else {
			sect.addRelocationEntry(locationCounter, type, sym.section);
			return sym.value;
		}
	}
	else {
		
	}
}

void Assembler::handleInstruction(Instruction* inst){

}

void Assembler::handleDirective(Directive * dir,std::ofstream& out){

	std::regex sectionRegex("^(data|text|bss|section)$");
	std::string name = dir->getName();
	if (std::regex_search(name, sectionRegex)) {
		// reset location counter and change current section
		locationCounter = 0;
		if (name == "section") currentSection = dir->getParam();
		else currentSection = "." + name;
		out << std::endl << "#" + currentSection << std::endl;
	}
	else if (name == "global" || name == "extern") {
		std::string param = dir->getParam();
		std::queue<std::string> paramTokens = util::tokenize(param, ",");
		while (!paramTokens.empty()) {
				std::string symName = paramTokens.front();
				paramTokens.pop();
				symbolTable.at(symName).type = Symbol::Type::GLOBAL;
		}
	}
	else if (name == "byte" || name == "word") {
		std::string param = dir->getParam();
		if (name == "word" && locationCounter & 1) { out << "00 ";  locationCounter++; }// if word aligment is neccessary
		short tmp = 0;
		if (std::regex_search(param, Parser::tokenParsers.at("val"))) tmp = util::convertStringToDecimal(param);
		else tmp = handleSymbol(param, RelocationEntry::Type::R_386_32);
		out << util::convertDecimalToString(tmp, name == "byte" ? true : false) << " ";
		locationCounter += dir->getSize() - 1;
	}
	else {
		locationCounter += dir->getSize();
	}
}



bool Assembler::assemble(){
	try {
		Symbol und("UND", 0, 0, Symbol::Type::LOCAL, 0);
		std::string undefined = "UND";
		symbolTable.insert({ undefined,und });
		
		firstRun();

		secondRun();

	}
	catch (util::AssemblerException e) {
		std::cout << e << std::endl;
	}
	return false;
}
