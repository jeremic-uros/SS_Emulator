#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#include <cstdint>
#include <string>

class Program;

class Emulator {
private:
	static const size_t MEM_SIZE = 0x10000;
	static const uint8_t NUMBER_OF_REGISTERS = 8;
	static const size_t STACK_SIZE = 2 ^ 11; 
	static const size_t STACK_START = 0xFF00; // points to last occupied slot and goes down
	static const size_t MAPPED_REG_START = 0xFF00;
	static const size_t IVTP = 0x0000;
	static const size_t IVT_SIZE = 2 * 8;
	static const size_t MAX_PROG_SIZE = 0xF000;
	
	Program* program;
	uint16_t programStart;
	size_t programSize;

	uint8_t memory[MEM_SIZE]; 
	struct {
		int16_t R0;
		int16_t R1;
		int16_t R2;
		int16_t R3;
		int16_t R4;
		int16_t R5;
		union {
			int16_t R6;
			int16_t SP;
		};
		union {
			int16_t R7;
			int16_t PC;
		};

	}registers;
	union {
		uint16_t val;
		struct {
			uint16_t I : 1, Tl : 1, Tr : 1, : 9, N : 1, C : 1, O : 1, Z : 1;
		};
	} PSW;

	void systemInit();

	void regWrite(uint16_t val, uint8_t ind);
	int16_t regRead(uint8_t ind);
	
	void memWrite(uint8_t val, size_t off);
	void memWriteWord(uint16_t val, size_t off);
	uint8_t memRead(size_t off);
	uint16_t memReadWord(size_t off);

	void stackPush(uint8_t val);
	void stackPushWord(uint16_t val);
	int8_t stackPop();
	int16_t stackPopWord();

	void loadProgramFromFile(std::string filePath);

	friend class Program;
public:
	Emulator();
	~Emulator();
	Emulator(const Emulator& emu);

	Emulator& operator= (const Emulator& emu);
	
	void emulate(std::string filePath);


};
#endif // !_EMULATOR_H_

