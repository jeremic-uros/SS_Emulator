#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#include <cstdint>
#include <string>
#include <unordered_map>
#include <thread>
#include <chrono>

class Program;
class Section;
class Symbol;

class Emulator {
private:
	static const size_t MEM_SIZE = 0x10000;
	static const uint8_t NUMBER_OF_REGISTERS = 8;
	static const uint16_t STACK_SIZE = 0x800; 
	static const uint16_t STACK_START = 0xFF00; // points to last occupied slot and goes down
	static const uint16_t MAPPED_REG_START = 0xFF00;
	static const uint16_t IVTP = 0x0000;
	static const uint16_t IVT_SIZE = 0x10;
	static const uint16_t MAX_PROG_SIZE = 0xF000;
	static const uint16_t DEFAULT_PROG_OFFSET = 0x1000;
	static const uint16_t PROGRAM_LIMIT = 0xF700;
	static const uint16_t TERMINAL_DATA_OUT_REG = 0xFF00;
	static const uint32_t TERMINAL_DATA_IN_REG = 0xFF02;
	static const uint16_t TIMER_CFG_REG = 0xFF10;
	static const uint16_t SYSTEM_ROUTINES_START = 0x12;
	static const uint16_t SYSTEM_RESERVED_END = 0x100;
	static const uint16_t USER_PROGRAM_START_LOCATION = 0x10;
	static std::unordered_map<uint8_t, uint8_t> timerDurations; // for diffrent values of TIMER_CFG_REG

	bool init;
	uint16_t systemLocationCounter;
	uint8_t IVCounter;
	Program* program;
	uint16_t programStart;
	size_t programSize;
	std::unordered_map<std::string, uint16_t> startAddrs;

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
	void loadIVT();

	void regWrite(uint16_t val, uint8_t ind);
	int16_t regRead(uint8_t ind);
	
	void memWrite(uint8_t val, uint16_t off);
	void memWriteWord(uint16_t val, uint16_t off);
	uint8_t memRead(uint16_t off);
	uint16_t memReadWord(uint16_t off);

	void stackPush(uint8_t val);
	void stackPushWord(uint16_t val);
	int8_t stackPop();
	int16_t stackPopWord();

	void loadProgramFromFile(std::string filePath);
	void placeSectionsAndFix(std::unordered_map<std::string, Section>&, std::unordered_map<unsigned short, Symbol>&,uint8_t*);

	static void keyboardThreadRun();
	static volatile char keyboardBuffer;
	static volatile bool keyboardInterrupt;
	static volatile bool lastCharRead;

	static volatile bool dataReady; // used for output
	std::thread* keyboardThread;


	std::chrono::time_point<std::chrono::system_clock> lastTimerInterrupt;

	friend class Program;
public:
	Emulator();
	~Emulator();
	Emulator(const Emulator& emu);

	Emulator& operator= (const Emulator& emu);
	
	void emulate(std::string filePath,std::unordered_map<std::string,uint16_t>&);


};
#endif // !_EMULATOR_H_

