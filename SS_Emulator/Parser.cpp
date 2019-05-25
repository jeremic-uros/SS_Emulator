#include "Parser.h"
#include "Instruction.h"
#include "ParserException.h"
#include <iostream>

Parser* Parser::parser = nullptr;


std::unordered_map<std::string, std::regex> Parser::tokenParsers = {
	{"label", std::regex("^[0-9a-z]+:$")},
	{"instruction", std::regex("^(halt|xchg|int|mov|add|sub|mul|div|cmp|not|and|or|xor|test|shl|shr|push|pop|jmp|jeq|jne|jgt|call|ret|iret)(b|w)?$")},
	{"directive", std::regex("^\.(text|data|bss|equ|global|extern|section|byte|word|align|skip|end)$")},
};


Parser * Parser::getParser(){
	if (parser == nullptr) { parser = new Parser(); }
	return Parser::parser;
}

Parser::Parser(){} // default constructor

ParsedLine* Parser::parse(std::queue<std::string>* tokens)
{
	if (tokens->empty()) { throw new util::ParserException("PARSING ERROR: No tokens"); }
	// take first token to check if labeled
	std::string token = tokens->front();
	tokens->pop();
	std::string label = isLabeled(token);

	// if line is labeled move to the next token
	if (label.size() >= 1) {
		if(tokens->empty()) { throw new util::ParserException("PARSING ERROR: Not enough tokens, invalid line"); }
		token = tokens->front();
		tokens->pop();
	}

	// Proccess next token to determine if line is a instruction or a directive or invalid
	unsigned char type = parseType(token);
	//if (type == ParsedLine::Types::Inv) throw new util::ParserException("PARSING ERROR: Instruction or directive not recoqnized"); // stop executing 



	Instruction* parsedLine = new Instruction();
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
	if (std::regex_search(token, match, tokenParsers.at("directive"))) ret = ParsedLine::Types::Dir;
	return ret;
}


