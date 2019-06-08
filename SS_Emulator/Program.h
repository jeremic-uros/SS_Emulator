#ifndef _PROGRAM_H_
#define _PROGRAM_H_
#include <cstdint>

class Program {
private:
	uint8_t InstructionDescr;
	uint8_t Opr1Descr;
	uint16_t Opr1Field;
	uint8_t Opr2Descr;
	uint16_t Opr1Field;

	void fetch(); 
	void decode();
	void execute();	
	void handleInterrupts(); // delagates control to emulator

public:

	void run(); 

};
#endif // !_PROGRAM_H_

