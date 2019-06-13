
#include "Linker.h"
#include "Emulator.h"
#include "Util.h"
#include "Assembler.h"
#include <string>
#include <iostream>
#include <regex>
#include <list>
#include <unordered_map>
#include <cstdint>

using namespace std;


int main(int argc, char *argv[]) {

	try {
		// FOR TESTING ONLY
		Assembler assembler("input.txt","input&outputTest1.o");
		assembler.assemble();
		Assembler assembler1("input1.txt", "input&outputTest2.o");
		assembler1.assemble();
		///////////////////////////////////////
		if (argc < 2) {
			cerr << "Wrong number of parameters.\n Usage: emulator [-place=<section_name>@<location>] inputfiles\n";
			return -1;
		}

		regex placeRegex("-place=[a-z]+@[x0-9]+");
		list<string> filenames;
		unordered_map<string, uint16_t> placeArgs;
		int args = 1;
		while(args < argc){
			string arg = argv[args];
			
			if (regex_search(arg,placeRegex)) {
				arg = arg.substr(7, arg.npos);
				queue<string> tokens = util::tokenize(arg, "@");
				string sect = "."+tokens.front();
				tokens.pop();
				placeArgs.insert({ sect,util::convertStringToDecimal(tokens.front()) });
			}
			else {
				filenames.push_back(arg);
			}
			args++;
		}

		Linker linker(filenames, "out.exec");
		linker.link();

		Emulator emulator;
		emulator.emulate("out.exec", placeArgs);
		
	}
	catch (std::runtime_error e) {
		cerr << e.what() << endl;
	}

	int x;
	cin >> x;
	return 0;
}
