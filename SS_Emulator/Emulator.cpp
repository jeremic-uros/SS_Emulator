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

void Emulator::memWrite(uint8_t val, size_t off){
	memory[off] = val;
}

uint8_t Emulator::memRead(size_t off)
{
	return memory[off];
}

void Emulator::loadProgramFromFile(std::string filePath){
	program = new Program(*this);
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


	for (int i = 0; i < size; i++)
		std::cout << util::convertDecimalToString(memory[i], true) << " ";
	delete data;

	in.close();
}

Emulator::Emulator(){
	memory = new uint8_t[MEM_SIZE];
}

Emulator::~Emulator(){
	delete program;
	delete memory;
}

void Emulator::emulate(std::string filePath){
	loadProgramFromFile(filePath);
}
