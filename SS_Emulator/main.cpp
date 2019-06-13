
#include "AssemblerException.h"
#include "Assembler.h"
#include <string>
#include <iomanip>
#include <fstream>
#include <cstdint>
#include <stdint.h>
#include <stdio.h>

using namespace std;


int main(int argc, char *argv[]) {

	try {
		if (argc != 4) {
			cerr << "Wrong number of parameters.\n Usage: assembler -o outputfile inputfile\n";
			return -1;
		}
		std::string inputFile;
		std::string outputFile;
		int args = 1;
		while(args < argc){
			string arg = argv[args];
			if (arg == "-o") {
				args++;
				if (args == argc) {
					cerr << "Expected file name after -o. \n Usage: assembler -o outputfile inputfile\n";
					return -1;
				}
				outputFile = argv[args];
			}
			else {
				inputFile = argv[args];
			}
			args++;
		}
		Assembler asembler(inputFile,outputFile);
		asembler.assemble();
	}
	catch (util::AssemblerException e) {
		cerr << e << endl;
	}
	catch (std::runtime_error e) {
		cerr << e.what() << endl;
	}
	return 0;
}
