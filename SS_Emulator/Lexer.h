#pragma once
#ifndef _LEXER_H_
#define _LEXER_H_
#include <queue>


class Lexer {
private:
	static Lexer* lexer;
	Lexer();
public:
	static Lexer* getLexer();
	std::queue<std::string>* tokenize(std::string line); // split the input line into string tokens for parsing
};

#endif // !_LEXER_H_
