#ifndef _PROGRAM_H_
#define _PROGRAM_H_
#include <cstdint>
#include <string>
#include <unordered_map>
#include <functional>

class Emulator;

class Program {
private:
	Emulator& emulator;


	std::string instName;
	uint8_t instructionDescr;
	uint8_t opr1Descr;
	int16_t opr1Val;
	uint8_t opr2Descr;
	int16_t opr2Val;


	void readInstruction(); 
	int16_t decodeVal(uint8_t oprDescr, int16_t oprVal);
	size_t decodeAdr(uint8_t oprDescr, int16_t oprVal); // returns the number of reg or mem offset 
	void execute();

	void handleInterrupts(); // delagates control to emulator


	// instruction executors
	
	void HALT_EXECUTOR();
	void XCHG_EXECUTOR();
	void INT_EXECUTOR();
	void MOV_EXECUTOR();
	void SUB_EXECUTOR();
	void ADD_EXECUTOR();
	void MUL_EXECUTOR();
	void DIV_EXECUTOR();
	void CMP_EXECUTOR();
	void NOT_EXECUTOR();
	void AND_EXECUTOR();
	void OR_EXECUTOR();
	void XOR_EXECUTOR();
	void TEST_EXECUTOR();
	void SHL_EXECUTOR();
	void SHR_EXECUTOR();
	void PUSH_EXECUTOR();
	void POP_EXECUTOR();
	void JMP_EXECUTOR();
	void JEQ_EXECUTOR();
	void JNE_EXECUTOR();
	void JGT_EXECUTOR();
	void CALL_EXECUTOR();
	void RET_EXECUTOR();
	void IRET_EXECUTOR();


	bool running;

public:
	Program(Emulator& emu) : emulator(emu) {}

	void run(); 

};
#endif // !_PROGRAM_H_

