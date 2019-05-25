#include "Lexer.h"
#include <regex>
#include <algorithm>
#include <iostream>


Lexer* Lexer::lexer = nullptr;

Lexer::Lexer() {
	// private constructor
}

Lexer* Lexer::getLexer() {
	if (lexer == nullptr) { lexer = new Lexer(); }
	return lexer;
}

std::queue<std::string>* Lexer::tokenize(std::string line)
{
	std::queue<std::string>* tokens = new std::queue<std::string>();
	std::transform(line.begin(), line.end(), line.begin(), ::tolower); // all letters to lower case for ease
	std::string token = "";
	try {
		std::smatch match;
		std::regex allowedCharacters("[(a-z0-9\[&$*:\\]\.)]+");
		while (std::regex_search(line, match, allowedCharacters)) {
			token = match.str(0);
			tokens->push(token);
			line = match.suffix().str();
		}
	}
	catch (std::regex_error& e) {
		std::cout << e.what() << std::endl; 
	}

	return tokens;
}
