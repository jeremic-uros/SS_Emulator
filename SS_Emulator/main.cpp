
#include "Lexer.h"
#include "Parser.h"
#include "AssemblerException.h"
#include "Directive.h"
#include "Instruction.h"
#include "Linker.h"
#include "Assembler.h"
#include "Emulator.h"
#include "Util.h"
#include <string>
#include <iostream>
#include <queue>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <stdint.h>

using namespace std;


int main(int argc, char *argv[]) {

	try {
		if (argc < 3) return -1;
		std::string inputFile = argv[1];
		std::string outputFile = argv[2];
		Assembler asembler(inputFile,outputFile);
		asembler.assemble();
	}
	catch (util::AssemblerException e) {
		cerr << e << endl;
	}
	catch (std::runtime_error e) {
		cerr << e.what() << endl;
	}
	int x;
	cin >> x;
	return 0;
}
