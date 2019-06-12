#include "Parser.h"
#include "Instruction.h"
#include "Directive.h"
#include "Util.h"
#include "AssemblerException.h"
#include <iostream>

Parser* Parser::parser = nullptr;

std::string valRegex = "([0-9]+|0x[0-9a-f]{1,4})";
std::string symbolRegex = "[a-z][a-z0-9]*";

std::unordered_map<std::string, std::regex> Parser::tokenParsers = {
	{"label", std::regex("^" + symbolRegex + ":$")},
	{"instruction", std::regex("^(halt|xchg|int|mov|add|sub|mul|div|cmp|not|and|or|xor|test|shl|shr|push|pop|jmp|jeq|jne|jgt|call|ret|iret)(b|w)?$")},
	{"directive", std::regex("^\\.(text|data|bss|equ|global|extern|section|byte|word|align|skip|end)$")},
	{"symbol", std::regex("^" + symbolRegex + "$")},
	{"val",std::regex("^-?" + valRegex + "$")},
	// instruction operands parsers
	{"regdir",std::regex("^((r[0-7])|sp|pc)(h|l)?$")},
	{"regin", std::regex("^(\\[(r[0-7]|sp|pc)\\])$")},
	{"reginx",std::regex("^r[0-7]\\[(-?" + valRegex + "|" + symbolRegex + ")\\]$")},
	{"pcrel",std::regex("^\\$" + symbolRegex + "$")},
	{"absolut", std::regex("^\\*" + valRegex + "$")},
	{"symbolImm",std::regex("^&" + symbolRegex + "$")},
	// directive params parsers
	{"expr", std::regex("^(" + valRegex + "|" + symbolRegex + ")([\\+\\-](" + valRegex + "|" + symbolRegex + "))*" + "$")},
	{"sectionName",std::regex("^\\." + symbolRegex + "$")},
	{"symbolArray", std::regex("^("+symbolRegex+"|,)$") }
};


Parser * Parser::getParser(){
	if (parser == nullptr) { parser = new Parser(); }
	return Parser::parser;
}

Parser::Parser(){} // default constructor

ParsedLine* Parser::parse(std::queue<std::string>* tokens){
	if (tokens->empty()) { return nullptr; }
	// take first token to check if labeled
	std::string token = tokens->front();
	tokens->pop();
	std::string label = isLabeled(token);

	// if line is labeled move to the next token
	if (label.size() >= 1) {
		if (tokens->empty()) { return new ParsedLine(ParsedLine::Types::Label,label); }
		token = tokens->front();
		tokens->pop();
	}

	// Proccess next token to determine if line is a instruction or a directive or invalid
	unsigned char type = parseType(token);


	// Proccess next tokens by type
	std::string name;
	unsigned char instAttr = 0; // cant be in switch 
	ParsedLine* parsedLine = nullptr;
	switch (type){
	case ParsedLine::Types::Inst:
		name = parseInstructionName(token, instAttr);
		parsedLine = new Instruction(ParsedLine::Types::Inst, label, name);
		((Instruction*)parsedLine)->setOperandAttributes(instAttr);
		parseInstructionOperands(tokens,(Instruction*)parsedLine);
		break;
	case ParsedLine::Types::Dir:
		name = token.substr(1, token.size());
		parsedLine = new Directive(ParsedLine::Types::Dir, label, name);
		parseDirective(tokens, (Directive*) parsedLine);
		break;
	default:
		throw util::AssemblerException("PARSING ERROR: Instruction or directive not recoqnized"); // stop executing 
		break;
	}


	return parsedLine;
}

std::string Parser::isLabeled(std::string token){
	std::string ret = "";
	std::smatch match;
	if (std::regex_search(token, match, tokenParsers.at("label"))) {
		ret = match.str(0);
		ret.pop_back();
	}
	return ret;
}

unsigned char Parser::parseType(std::string token){
	unsigned char ret = ParsedLine::Types::Inv;
	std::smatch match;
	if (std::regex_search(token, match, tokenParsers.at("instruction"))) ret = ParsedLine::Types::Inst;
	else if (std::regex_search(token, match, tokenParsers.at("directive"))) ret = ParsedLine::Types::Dir;
	return ret;
}

std::string Parser::parseInstructionName(std::string token, unsigned char & instAttr){
	std::string temp = token;
	temp.pop_back();
	if (std::regex_search(temp, tokenParsers.at("instruction"))) {
		char oprSize = token[token.size() - 1];
		instAttr |= (oprSize == 'w' ? 1 : 0); // set operand size
		return temp;
	}
	else {
		instAttr |= 1;
		return token;
	}

}

void Parser::parseInstructionOperands(std::queue<std::string>* tokens, Instruction * inst){
	unsigned char tokensToProcess = Instruction::numOfOperands.at(inst->getName());
	unsigned char instructionSize = 1 + tokensToProcess * 3; // InstDescr + numOfOperands * (OprDesr+2 bytes for field)
	if (tokensToProcess == 2) tokensToProcess++;
	if (tokens->size() != tokensToProcess) throw util::AssemblerException("PARSING ERROR: To many or to few tokens");
	

	bool first = true;
	while (tokensToProcess > 0) {
		std::string token = tokens->front();
		std::string field = "";
		unsigned char addr = -1;
		tokens->pop();
		tokensToProcess--;

		// Parse each addresing mode 
		if (std::regex_search(token, tokenParsers.at("regdir"))) {
			char lastChar = token[token.size() - 1];
			unsigned char instAttr = inst->getOperandAttributes();
			if (lastChar == 'h') {
				if(first) instAttr |= 2;
				else instAttr |= 4;
				inst->setOperandAttributes(instAttr);
				token.pop_back();
			}
			else if (lastChar == 'l') {
				token.pop_back();
			}
			field = token;
			if (field == "sp") { field = "r6"; }
			if (field == "pc") { field = "r7"; }
			addr = Instruction::addressingCodes.at("regdir");
			instructionSize -= 2; // only one byte used for operand
		}
		else if (std::regex_search(token, tokenParsers.at("regin"))) {
			field = token.substr(1, 2);
			if (field == "sp") { field = "r6"; }
			if (field == "pc") { field = "r7"; }
			addr = Instruction::addressingCodes.at("regin");
			instructionSize -= 2; // only one byte used for operand
		}
		else if (std::regex_search(token, tokenParsers.at("reginx"))) {
			token.pop_back();
			std::string comma = ",";
			field = token.replace((size_t)2,(size_t)1,comma);
			if(util::convertStringToDecimal(field.substr(3, field.npos)) >> 8 ) addr = Instruction::addressingCodes.at("reginx16");
			else {
				addr = Instruction::addressingCodes.at("reginx8");
				instructionSize--;
			}
			 
		}
		else if (std::regex_search(token, tokenParsers.at("val"))) {
			field = token;
			addr = Instruction::addressingCodes.at("imm");
			if (!(inst->getOperandAttributes() & 1)) instructionSize--;
		}
		else if (std::regex_search(token, tokenParsers.at("symbol"))) {
			field = token;
			addr = Instruction::addressingCodes.at("mem");
		}
		else if (std::regex_search(token, tokenParsers.at("symbolImm"))) {
			field = token.substr(1, token.size() - 1);
			addr = Instruction::addressingCodes.at("imm");
		}
		else if (std::regex_search(token, tokenParsers.at("pcrel"))) {
			field = token.substr(1, token.size() - 1);
			unsigned char instAttr = inst->getOperandAttributes();
			inst->setOperandAttributes(instAttr | 8); // set pc rel flag, for relocation usage
			addr = Instruction::addressingCodes.at("reginx16");
		}
		else if (std::regex_search(token, tokenParsers.at("absolut"))) {
			field = token.substr(1, token.size() - 1);
			addr = Instruction::addressingCodes.at("mem");
		}
		else throw util::AssemblerException("PARSING ERROR: Invalid operand");

		if (first) {
			inst->setFirstOprField(field);
			inst->setFirstOprAddr(addr);
			first = false;
			if (tokensToProcess > 0) {
				tokensToProcess--;
				if (tokens->front() == ",") tokens->pop();
				else throw util::AssemblerException("PARSING ERROR: Expected ',' after first operand");
			}
		}
		else {
			inst->setSecondOprField(field);
			inst->setSecondOprAddr(addr);
		}

	}

	inst->setSize(instructionSize);
}

void Parser::parseDirective(std::queue<std::string>* tokens, Directive * dir) {
	std::string name = dir->getName();
	unsigned char tokensToProcess = Directive::numOfParams.at(name);
	if (name == "equ") tokensToProcess++;
	if (tokens->size() != tokensToProcess) { 
		if ( name != "global" && name != "extern") throw util::AssemblerException("PARSING ERROR: To many or to few tokens"); 
	}

	std::string param = "";
	tokensToProcess = tokens->size(); // hack beacuse of global and extern directives
	bool first = true; // used for equ
	while (tokensToProcess > 0) {
		std::string token = tokens->front();
		tokens->pop();
		tokensToProcess--;

		
		if (name == "equ") {
			if (first && std::regex_search(token, tokenParsers.at("symbol"))) {
				param = token + ",";
				first = false;
				//Procces the ',' token
				tokensToProcess--;
				if (tokens->front() == ",") tokens->pop();
				else util::AssemblerException("PARSING ERROR: Expected ','");
			}
			else if (std::regex_search(token, tokenParsers.at("val"))) {
				param += token;
			}
		}
		else {
			std::string parser = Directive::directiveParsingGroup.at(name);
			if (std::regex_search(token, tokenParsers.at(parser))) {
				if (name == "global" || name == "extern") {
					param += token;
				}
				else param = token;
			}
			else throw util::AssemblerException("PARSING ERROR: Invalid directive param");
		}
	}
	unsigned char size = 0;
	if (name == "word") size = 2;
	else if (name == "byte") size = 1;
	else if (name == "skip") {
		size = util::convertStringToDecimal(param);
	}
	dir->setSize(size);
	// align has to be determined by the assembler
	dir->setParam(param);
}


