#ifndef _PROGRAM_H_
#define _PROGRAM_H_
#include <cstdint>

class Emulator;

class Program {
private:
	Emulator& emulator;

	uint8_t instructionDescr;
	uint8_t opr1Descr;
	uint16_t opr1Val;
	uint8_t opr2Descr;
	uint16_t opr2Val;

	void fetch(); 
	void decode();
	void execute();	
	void handleInterrupts(); // delagates control to emulator

	bool running;

public:
	Program(Emulator& emu) : emulator(emu) {}

	void run(); 

};
#endif // !_PROGRAM_H_

