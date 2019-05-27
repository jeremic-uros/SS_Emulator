#include "Parser.h"
#include "Instruction.h"
#include "AssemblerException.h"
#include <iostream>

Parser* Parser::parser = nullptr;

std::string valRegex = "([0-9]+|0x[0-9]{1,4})";
std::string symbolRegex = "[a-z][a-z0-9]*";

std::unordered_map<std::string, std::regex> Parser::tokenParsers = {
	{"label", std::regex("^"+symbolRegex+":$")},
	{"instruction", std::regex("^(halt|xchg|int|mov|add|sub|mul|div|cmp|not|and|or|xor|test|shl|shr|push|pop|jmp|jeq|jne|jgt|call|ret|iret)(b|w)?$")},
	{"directive", std::regex("^\.(text|data|bss|equ|global|extern|section|byte|word|align|skip|end)$")},
	{"val",std::regex("^"+valRegex+"$")},
	{"regdir",std::regex("^((r[0-7])|sp|pc)(h|l)?$")},
	{"regin", std::regex("^(\\[(r[0-7]|sp|pc)\\])$")},
	{"reginx",std::regex("^r[0-7]\\[("+valRegex+"|"+symbolRegex+")\\]$")},
	{"pcrel",std::regex("^(\\$|&)"+symbolRegex+"$")},
	{"symbol", std::regex("^"+symbolRegex+"$")},
	{"absolut", std::regex("^\\*"+valRegex+"$")}

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
		parsedLine = new Instruction(ParsedLine::Types::Inst, label);
		((Instruction*)parsedLine)->setOperandAttributes(instAttr);
		((Instruction*)parsedLine)->setName(name);
		parseInstructionOperands(tokens,(Instruction*)parsedLine);
		break;
	case ParsedLine::Types::Dir:

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
	//std::smatch match;
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
	unsigned char tokensToProcess = codes::numOfOperands.at(inst->getName());
	if (tokens->size() != tokensToProcess) throw util::AssemblerException("PARSING ERROR: To many or to few tokens");
	unsigned char instructionSize = 3 + tokensToProcess * ((inst->getOperandAttributes() & 1) + 1); // (InstDescr+2*OprDescr) + numOfOperands * operandSize

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
			addr = codes::addressingCodes.at("regdir");
			instructionSize -= (instAttr & 1) + 1;
		}
		else if (std::regex_search(token, tokenParsers.at("regin"))) {
			field = token.substr(1, 2);
			if (field == "sp") { field = "r6"; }
			if (field == "pc") { field = "r7"; }
			addr = codes::addressingCodes.at("regin");
			instructionSize -= (inst->getOperandAttributes() & 1) + 1;
		}
		else if (std::regex_search(token, tokenParsers.at("reginx"))) {
			token.pop_back();
			std::string blank = " ";
			field = token.replace((size_t)2,(size_t)1,blank);
			if (inst->getOperandAttributes() & 1)	addr = codes::addressingCodes.at("reginx16");
			else addr = codes::addressingCodes.at("reginx8");		
		}
		else if (std::regex_search(token, tokenParsers.at("val"))) {
			field = token;
			addr = codes::addressingCodes.at("imm");
		}
		else if (std::regex_search(token, tokenParsers.at("symbol"))) {
			field = token;
			addr = -1; // TO BE UPDATED
		}
		else if (std::regex_search(token, tokenParsers.at("pcrel"))) {
			field = token.substr(1, token.size() - 1);
			addr = -1; // TO BE UPDATED
		}
		else if (std::regex_search(token, tokenParsers.at("absolut"))) {
			field = token.substr(1, token.size() - 1);
			addr = codes::addressingCodes.at("mem");
		}
		else throw util::AssemblerException("PARSING ERROR: Invalid operand");

		if (first) {
			inst->setFirstOprField(field);
			inst->setFirstOprAddr(addr);
			first = false;
		}
		else {
			inst->setSecondOprField(field);
			inst->setSecondOprAddr(addr);
		}

	}

	inst->setSize(instructionSize);
}


