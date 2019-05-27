#include "Lexer.h"
#include "AssemblerException.h"
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
	int numOfMatchedChars = 0; // used to test if there are more chars than there should be
	try {
		std::smatch match;
		std::regex allowedCharacters("[(a-z0-9\[&$*:\\]\.\\+\\-)]+");
		std::string temp = line;
		while (std::regex_search(temp, match, allowedCharacters)) {
			token = match.str(0);
			numOfMatchedChars += token.size();
			tokens->push(token);
			temp = match.suffix().str();
			
		}

		// Needs refactoring, and a another design possibly
		//int t = (line.find(',') == std::string::npos) ? 0 : 1;
		int numOfWhiteSpace = std::count(line.begin(), line.end(), ' ');
		int numOfTabs = std::count(line.begin(), line.end(), '	');
		int numOfPoints = std::count(line.begin(), line.end(), ',');
		if (numOfMatchedChars != (line.length() - numOfWhiteSpace - numOfTabs - numOfPoints)) throw util::AssemblerException("LEXER ERROR: To many unallowed characters");

	}
	catch (std::regex_error& e) {
		std::cout << e.what() << std::endl; 
	}

	return tokens;
}
