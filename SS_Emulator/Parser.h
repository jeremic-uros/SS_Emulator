#ifndef _PARSER_H_
#define _PARSER_H_
#include <string>
#include <queue>
#include <unordered_map>
#include <regex>

class ParsedLine;


class Parser {
private:
	static std::unordered_map<std::string, std::regex> tokenParsers; // map to store all regex used for parsing
	static Parser* parser;
	Parser();
public:
	static Parser* getParser();	// to avoid memmory loss only one parser can be made
	ParsedLine* parse(std::queue<std::string>* tokens); // tokens provided from the lexer
	std::string isLabeled(std::string token);	// check if token is a label and returns name, otherwise returns null
	unsigned char parseType(std::string token); // used on first or second token if labeled to determine Type
};

#endif  

