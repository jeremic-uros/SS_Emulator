
#include "Lexer.h"
#include "Parser.h"
#include "AssemblerException.h"
#include "Directive.h"
#include "Instruction.h"
#include "Linker.h"
#include "Assembler.h"
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

		//Assembler asembler("input1.txt","output1.o");
		//asembler.assemble();

		std::string * fileNames = new std::string[2];
		fileNames[0] = "output.o";
		fileNames[1] = "output1.o";
		Linker linker(fileNames,2);
		linker.link();
		/*ifstream in("output.o");
		string line;
		while (getline(in, line) && line != "#.data"){}
		uint8_t bytes[3];
		in.read((char*)bytes, 3);
		getline(in, line);
		uint8_t bytes2[32];
		in.read((char*)bytes2, 32);
		getline(in, line);
		uint8_t bytes3[13];
		in.read((char*)bytes3, 13);
		getline(in, line);
		uint16_t bss = 0;
		uint8_t bytes4[2];
		in.read((char*)bytes4, 2);
		bss =  bytes4[1] << 8 | bytes4[0];
		cout << bss;

		in.close();*/
	}
	catch (util::AssemblerException e) {
		cerr << e << endl;
	}


	int x;
	cin >> x;
	return 0;
}