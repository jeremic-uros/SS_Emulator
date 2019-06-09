
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
int main() {

	try {

		Assembler asembler("input.txt","output.o");
		asembler.assemble();
		Assembler asembler1("input1.txt", "output1.o");
		asembler1.assemble();


		std::string * fileNames = new std::string[2];
		fileNames[0] = "output.o";
		fileNames[1] = "output1.o";
		Linker linker(fileNames,2,"out.exec");
		linker.link();

		Emulator emulator;
		emulator.emulate("out.exec");
	}
	catch (util::AssemblerException e) {
		cerr << e << endl;
	}


	int x;
	cin >> x;
	return 0;
}