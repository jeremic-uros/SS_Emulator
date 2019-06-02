
#include "Lexer.h"
#include "Parser.h"
#include "AssemblerException.h"
#include "Directive.h"
#include "Instruction.h"
#include "Assembler.h"
#include "Util.h"
#include <string>
#include <iostream>
#include <queue>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cstdint>

using namespace std;
int main() {

	Lexer* lexer = Lexer::getLexer();
	string testString1 = ".global a,b,c,d,s,e";
	string testString2 = "lopticeeee: .equ a,    23";
	string testString3 = "a: push r3";

	try {

		Assembler asembler("input.txt","output.txt");
		asembler.assemble();
	}
	catch (util::AssemblerException e) {
		cerr << e << endl;
	}


	int x;
	cin >> x;
	return 0;
}