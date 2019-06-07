#include "Assembler.h"
#include "Lexer.h"
#include "AssemblerException.h"
#include "Parser.h"
#include "Instruction.h"
#include "Directive.h"
#include "Util.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <regex>
#include <cstdint>

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
				//else if ( name == "word" && locationCounter & 1 )  parsedLine->setSize(3);
				locationCounter += size;
			}

			// check if bss section contains only skip directive
			if (currentSection == ".bss" && !std::regex_search(name, std::regex("(end|skip|bss)"))) throw util::AssemblerException("ASSEMBLING ERROR: Only skip directive allowed in bss section");
			
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
		nextInstructionLocation = locationCounter + size;

		if (name == "end") { assembling = false; }
		// Directive handling
		if (type == ParsedLine::Types::Dir) handleDirective((Directive*)parsedLine,out);
		else if(type == ParsedLine::Types::Inst) handleInstruction((Instruction*)parsedLine, out);
	

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
	if (type == RelocationEntry::Type::R_386_32 || type == RelocationEntry::Type::R_386_N32) {
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
		// if symbol is in same section no need for relocation
		if (sym.section == sect.rb) return sym.value - nextInstructionLocation;


		if (sym.type == Symbol::Type::GLOBAL) {
			sect.addRelocationEntry(locationCounter, type, sym.rb);
			return locationCounter - nextInstructionLocation; // linker doesnt need to know instruction length
		}
		// symbol is local
		else {
			sect.addRelocationEntry(locationCounter, type, sym.section);
			return (locationCounter - nextInstructionLocation) + sym.value;
		}
	}
}

void Assembler::handleInstruction(Instruction* inst,std::ofstream& out){
	unsigned char instAttr = inst->getOperandAttributes();
	// Prepare and write instruction Descriptor
	unsigned char instDescr = Instruction::instructionCodes.at(inst->getName());
	instDescr = ((instDescr << 1) | (instAttr & 1)) << 2;
	out << util::convertDecimalToString(instDescr, true) << " ";
	locationCounter++;
	
	for (int i = 0; i < Instruction::numOfOperands.at(inst->getName()); i++) {
		if (i == 0) { // first operand
			handleOperand(inst->getFirstOprField(), inst->getFirstOprAddr(), instAttr , out);
			
		}
		else { // second operand
			handleOperand(inst->getSecondOprField(), inst->getSecondOprAddr(), instAttr | 16, out); // 5 bit of instAttr used to specify that it is the second opr
		}

		if (inst->getFirstOprAddr() == Instruction::AddrCodes::REGINX8 || inst->getSecondOprAddr() == Instruction::AddrCodes::REGINX8) inst->setSize(inst->getSize() - 1);
	}
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
		//if (name == "word" && locationCounter & 1) { out << "00 ";  locationCounter++; }// if word aligment is neccessary
		short tmp = 0;
		if (std::regex_search(param, Parser::tokenParsers.at("expr"))) {
			std::queue<std::string> tokens = util::tokenize(param, "+-");
			std::queue<std::string> operations = util::tokenize(param, "0-9a-z");
			
			bool first = true;
			while (!tokens.empty()) {
				std::string token = tokens.front();
				std::string operation;
				tokens.pop();
				if (first) {
					operation = "+";
					first = false;
				}
				else {
					operation = operations.front();
					operations.pop();
				}
				if (std::regex_search(token, Parser::tokenParsers.at("val"))) {
					if(operation == "+") tmp += util::convertStringToDecimal(token);
					else tmp -= util::convertStringToDecimal(token);
				}
				else if (std::regex_search(token, Parser::tokenParsers.at("symbol"))) {
					if(operation == "+") tmp += handleSymbol(token, RelocationEntry::Type::R_386_32);
					else tmp -= handleSymbol(token, RelocationEntry::Type::R_386_N32);
				}
			}
		}
		out << util::convertDecimalToString(tmp, name == "byte" ? true : false) << " ";
	}
	else if (name == "align" || name == "skip") {
		if(currentSection != ".bss")
			for (int i = 0; i < dir->getSize(); i++) out << "00 ";
	}
	locationCounter += dir->getSize();
}

void Assembler::handleOperand(std::string field, unsigned char addr, unsigned char attr,std::ofstream& out){
	unsigned char oprDescr = addr << 5;
	unsigned char lh = 0;
	unsigned char reg = 0;
	unsigned short val = 0;
	switch (addr){
	case Instruction::AddrCodes::IMM:
		out << util::convertDecimalToString(oprDescr, true) << " ";
		locationCounter++;
		if (std::regex_search(field, Parser::tokenParsers.at("val"))) {
			out << util::convertDecimalToString(util::convertStringToDecimal(field), (attr & 1) ? false : true) << " ";
			locationCounter += (attr & 1) ? 2 : 1;
		}
		else {
			out << util::convertDecimalToString(handleSymbol(field, RelocationEntry::Type::R_386_32), false) << " ";
			locationCounter += 2;
		}
		break;
	case Instruction::AddrCodes::REGDIR:
		lh = (attr & 16 ? (attr & 4) >> 2 : (attr & 2) >> 1); // get the low or high bit
	case Instruction::AddrCodes::REGIN:
		reg = util::convertStringToDecimal(field.substr(1, 1));
		reg = (reg << 1) | lh;
		oprDescr |= reg;
		out << util::convertDecimalToString(oprDescr, true) << " ";
		locationCounter++;
		break;
	case Instruction::AddrCodes::REGINX8:
		reg = util::convertStringToDecimal(field.substr(1, 1)) << 1;
		oprDescr |= reg;
		field = field.substr(3, field.npos);
		val = util::convertStringToDecimal(field);
		out << util::convertDecimalToString(oprDescr, true) << " ";
		out << util::convertDecimalToString(val, true) << " ";
		locationCounter += (val >> 2);
		
		break;
	case Instruction::AddrCodes::REGINX16:
		if (attr & 8) {
			reg = 7 << 1; // PC = R7
			oprDescr |= reg;
			out << util::convertDecimalToString(oprDescr, true) << " ";
			locationCounter++;
			out << util::convertDecimalToString(handleSymbol(field,RelocationEntry::Type::R_386_PC32), false) << " ";
			locationCounter += 2;
		}
		else { // field = r[0-9],<val>|<symbol>
			reg = util::convertStringToDecimal(field.substr(1, 1)) << 1;
			oprDescr |= reg;
			field = field.substr(3, field.npos);
			if (std::regex_search(field, Parser::tokenParsers.at("val"))) {
				val = util::convertStringToDecimal(field);			
				out << util::convertDecimalToString(oprDescr,true) << " ";
				out << util::convertDecimalToString(val,false) << " ";
				locationCounter += (val >> 3);
			}
			else {
				out << util::convertDecimalToString(oprDescr, true) << " ";
				locationCounter++;
				out << util::convertDecimalToString(handleSymbol(field,RelocationEntry::Type::R_386_32), false) << " ";
				locationCounter += 2;
			}
		}
		break;
	case Instruction::AddrCodes::MEM:
		out << util::convertDecimalToString(oprDescr, true) << " ";
		locationCounter++;
		if (std::regex_search(field, Parser::tokenParsers.at("val"))) out << util::convertDecimalToString(util::convertStringToDecimal(field), false) << " ";
		else out << util::convertDecimalToString(handleSymbol(field, RelocationEntry::Type::R_386_32), false) << " ";
		locationCounter += 2;
		break;
	default:
		break;
	}
}

void Assembler::formatedOutput(){
	std::ofstream out;
	out.open(outputFileName);

	// format and write section table
	out << "#Section table" << std::endl;
	out << std::left;
	out << std::setw(15) << "#name" << std::setw(10) << "size" << std::setw(10) << "rb" << std::endl;
	for (auto const& sec : sectionTable) 
		out << sec.second << std::endl;

	//	format and write symbol table
	out << "#Symbol table" << std::endl;
	out << std::setw(15) << "#name" << std::setw(10) << "section" << std::setw(10) << "value" << std::setw(10) << "type" << std::setw(10) << "rb" << std::endl;
	for (auto const& sym : symbolTable) 
		if (sym.first != "UND") 
			out << sym.second << std::endl;

	// format and write relocation tables
	for (auto const& sec : sectionTable) {
		if (sec.second.relocationTable) {
			out << std::setw(10) << "#.ret" + sec.second.name << std::endl;
			out << std::setw(10) << "#offset" << std::setw(12) << "type" << std::setw(10) << "symbol" << std::endl;
			for (auto const& rel : *sec.second.relocationTable)
				out << rel << std::endl;
		}
	}

	// copy from second work file
	std::ifstream in;
	in.open(workFileNameSecondRun);
	std::string line;
	while (std::getline(in, line)) {
		out << line << std::endl;
	}

	in.close();
	out.close();

}

void Assembler::formatForLinker(){
	std::ofstream out(outputFileName);
	// section table
	out << "#sectab" << std::endl;
	for (auto const& sec : sectionTable)
		out << sec.second << std::endl;
	// symbol table
	out << "#symtab" << std::endl;
	for (auto const& sym : symbolTable) {
		if (sym.first != "UND" && (sym.second.type == Symbol::Type::GLOBAL || sym.second.section == sym.second.rb)) {
			out << sym.second << std::endl;
		}
	}
	// ret tables
	for (auto const& sec : sectionTable) {
		if (sec.second.relocationTable) {
			out << std::setw(10) << "#.ret" + sec.second.name << std::endl;
			for (auto const& rel : *sec.second.relocationTable)
				out << rel << std::endl;
		}
	}

	// copy from second work file
	std::ifstream in;
	in.open(workFileNameSecondRun);
	std::string line;
	while (std::getline(in, line)) {
		if (std::regex_search(line, std::regex("#.*")) || line == "") out << line << std::endl;
		else {
			std::queue<std::string> bytesHex = util::tokenize(line, " ");
			while (!bytesHex.empty()) { // inefficent but works
				uint8_t byte = util::convertStringToDecimal("0x" + bytesHex.front());
				bytesHex.pop();
				out << byte;
			}
		}
	}
	in.close();
	out.close();
}



bool Assembler::assemble(){
	try {
		Symbol und("UND", 0, 0, Symbol::Type::LOCAL, 0);
		std::string undefined = "UND";
		symbolTable.insert({ undefined,und });
		
		firstRun();

		secondRun();

		//formatedOutput();
		formatForLinker(); 
	}
	catch (util::AssemblerException e) {
		std::cout << e << std::endl;
	}
	return false;
}
