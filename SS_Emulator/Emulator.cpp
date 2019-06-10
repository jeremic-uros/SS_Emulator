#include "Emulator.h"
#include "Program.h"
#include "ObjectFileReader.h"
#include "Util.h"
#include <unordered_map>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <cstring>

void Emulator::systemInit(){
	registers.SP = STACK_START;
}

void Emulator::regWrite(uint16_t val, uint8_t ind){
	switch (ind) {
	case 0:
		registers.R0 = val;
		break;
	case 1:
		registers.R1 = val;
		break;
	case 2:
		registers.R2 = val;
		break;
	case 3:
		registers.R3 = val;
		break;
	case 4:
		registers.R4 = val;
		break;
	case 5:
		registers.R5 = val;
		break;
	case 6:
		registers.R6 = val;
		break;
	case 7:
		registers.R7 = val;
		break;
	default:
		throw std::runtime_error("invalid registar number");
		break;
	}
}

int16_t Emulator::regRead(uint8_t ind)
{
	int16_t val;
	switch (ind){
	case 0:
		val = registers.R0;
		break;
	case 1:
		val = registers.R1;
		break;
	case 2:
		val = registers.R2;
		break;
	case 3:
		val = registers.R3;
		break;
	case 4:
		val = registers.R4;
		break;
	case 5:
		val = registers.R5;
		break;
	case 6:
		val = registers.R6;
		break;
	case 7:
		val = registers.R7;
		break;
	default:
		throw std::runtime_error("invalid registar number");
		break;
	}
	return val;
}

void Emulator::memWrite(uint8_t val, size_t off){
	memory[off] = val;
}

void Emulator::memWriteWord(uint16_t val, size_t off){
	uint8_t low = val & 0xFF;
	uint8_t high = val >> 8;
	memory[off] = low;
	memory[off + 1] = high;
}

uint8_t Emulator::memRead(size_t off){
	return memory[off];
}

uint16_t Emulator::memReadWord(size_t off){
	uint16_t val = memory[off + 1] << 8;
	val |= memory[off];
	return val;
}

void Emulator::stackPush(uint8_t val){
	registers.SP--;
	memWrite(val, registers.SP);
}

void Emulator::stackPushWord(uint16_t val){
	registers.SP -= 2;
	memWriteWord(val, registers.SP);
}

int8_t Emulator::stackPop()
{	
	int8_t ret = memRead(registers.SP);
	registers.SP++;
	return ret;
}

int16_t Emulator::stackPopWord(){
	int16_t ret = memReadWord(registers.SP);
	registers.SP += 2;
	return ret;
}

void Emulator::loadProgramFromFile(std::string filePath){
	std::unordered_map<unsigned short, Symbol> symTable; 
	std::unordered_map<std::string, Section> sectTable;
	std::ifstream in(filePath);


	ObjectFileReader::readSymbolandSectionTable(symTable, sectTable, in);

	uint16_t size = 0;
	for (auto sect : sectTable) {
		size += sect.second.size;
	}

	uint8_t* data = new uint8_t[size];

	in.read((char*)data, size);

	memcpy(memory, data, size);


	for (int i = 0; i < size; i++) {
		std::cout << util::convertDecimalToString(memory[i], true) << " ";
	}
	delete[] data;

	in.close();
}

Emulator::Emulator(){
	//memory = new uint8_t[MEM_SIZE];
}

Emulator::~Emulator(){
	delete program;
	//delete[] memory;
}

Emulator::Emulator(const Emulator & emu){

	program = new Program(*emu.program);
	/*memory = new uint8_t[MEM_SIZE];
	for(int i = 0 ; i < MEM_SIZE ; i++) {
		memory[i] = emu.memory[i];
	}*/
}

Emulator & Emulator::operator=(const Emulator & emu) {
	delete program;
	program = new Program(*emu.program);
	/*delete[] memory;
	memory = new uint8_t[MEM_SIZE];
	for (int i = 0; i < MEM_SIZE; i++) {
		memory[i] = emu.memory[i];
	}*/
	return *this;
}

void Emulator::emulate(std::string filePath){
	systemInit();	
	loadProgramFromFile(filePath);

	program = new Program(*this);
	registers.PC = 5; // hardcoded code start for testing purpose
	program->run();
}
