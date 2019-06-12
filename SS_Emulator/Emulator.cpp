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
#include <regex>
#include <thread>



volatile char Emulator::keyboardBuffer = '0';
volatile bool Emulator::keyboardInterrupt = false;
volatile bool Emulator::lastCharRead = true;
volatile bool Emulator::dataReady = false;

std::unordered_map<uint8_t, uint8_t> Emulator::timerDurations{
	{0, 0.5},
	{1, 1},
	{2, 1.5},
	{3, 2},
	{4, 5},
	{5, 10},
	{6, 30},
	{7, 60},
};

void Emulator::systemInit(){
	init = true;
	PSW.val = 0;
	loadIVT();

	//registers.SP = STACK_START; // move to routine
	//memory[TERMINAL_DATA_OUT_REG + 1] = 1; // move to routine
	//memWriteWord(0, TIMER_CFG_REG); // move to routine

	registers.PC = memReadWord(0); // first routine to run
	lastTimerInterrupt = std::chrono::system_clock::now();
	keyboardThread = new std::thread(Emulator::keyboardThreadRun);
	keyboardThread->detach();
	init = false;
}

void Emulator::loadIVT(){
	systemLocationCounter = SYSTEM_ROUTINES_START;
	IVCounter = 0;
	startAddrs.clear();
	loadProgramFromFile("cpuStartRoutine.exec");
	IVCounter += 2;
	startAddrs.clear();
	loadProgramFromFile("badInstructionRoutine.exec");
	IVCounter += 2;
	startAddrs.clear();
	loadProgramFromFile("defaultInterruptRoutine.exec");
	IVCounter += 2;
	startAddrs.clear();
	loadProgramFromFile("defaultInterruptRoutine.exec");
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

void Emulator::memWrite(uint8_t val, uint16_t off){
	if (off > MEM_SIZE) throw std::runtime_error("memory index out of bounds");
	memory[off] = val;
	if (off == TERMINAL_DATA_OUT_REG) {
		memory[off + 1] = 0;
		dataReady = true;
	}
}

void Emulator::memWriteWord(uint16_t val, uint16_t off){
	if (off > MEM_SIZE) throw std::runtime_error("memory index out of bounds");
	uint8_t low = val & 0xFF;
	uint8_t high = val >> 8;
	memory[off] = low;
	memory[off + 1] = high;
}

uint8_t Emulator::memRead(uint16_t off){
	if (off > MEM_SIZE) throw std::runtime_error("memory index out of bounds");
	uint8_t ret = memory[off];
	if (off == TERMINAL_DATA_IN_REG) {
		memory[off + 1] = 0;
		lastCharRead = true;
	}
	return ret;
}

uint16_t Emulator::memReadWord(uint16_t off){
	if (off > MEM_SIZE) throw std::runtime_error("memory index out of bounds");
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

void Emulator::loadProgramFromFile(std::string filePath) {
	std::unordered_map<unsigned short, Symbol> symTable;
	std::unordered_map<std::string, Section> sectTable;
	std::ifstream in(filePath);

	// read size and start point
	std::string programInfo;
	std::getline(in, programInfo);
	if (std::regex_search(programInfo, std::regex("^SIZE=[0-9]+ START_POINT=[0-9]+$"))) {
		std::queue<std::string> tokens = util::tokenize(programInfo, " ");
		std::string token = tokens.front();
		programSize = std::stoi(token.substr(5, token.npos));
		tokens.pop();
		token = tokens.front();
		programStart = std::stoi(token.substr(12, token.npos));

	}
	else throw std::runtime_error("Invalid file format");

	// read symbols and sections
	ObjectFileReader::readSymbolandSectionTable(symTable, sectTable, in);

	uint16_t size = 0;
	for (auto sect : sectTable) {
		if (sect.first != ".bss")
			size += sect.second.size;
	}

	uint8_t* data = new uint8_t[size];

	in.read((char*)data, size);

	ObjectFileReader::readRetTables(sectTable, in);
	in.close();

	// fix data
	placeSectionsAndFix(sectTable, symTable,data);

	// copy to memmory
	for (auto const& sect : sectTable) {
		if (sect.first != ".bss")
			memcpy(memory + startAddrs.at(sect.first), data + symTable.at(sect.second.rb).value, sect.second.size);
	}

	if (sectTable.find(".bss") != sectTable.end()) {
		uint16_t bssSize = sectTable.at(".bss").size;
		uint16_t bssStart = startAddrs.at(".bss");
		for (int i = bssStart; i < bssStart + bssSize; i++)
			memory[i] = 0;
	}

	delete[] data;



	in.close();
}


void Emulator::placeSectionsAndFix(std::unordered_map<std::string, Section>& sectTable, std::unordered_map<unsigned short, Symbol> & symTable,uint8_t* data){
	// check if sections overlap
	size_t highestLocation = init ? systemLocationCounter : DEFAULT_PROG_OFFSET;
	for (auto const& startAddr : startAddrs) {
		if (sectTable.find(startAddr.first) == sectTable.end()) throw std::runtime_error("No such section " + startAddr.first);
		if (startAddr.second < SYSTEM_RESERVED_END) throw std::runtime_error("sections must be placed above " + SYSTEM_RESERVED_END);
		size_t endAddr = startAddr.second + sectTable.at(startAddr.first).size;
		if (highestLocation < endAddr) highestLocation = endAddr;
		for (auto const& startAddrToCompare : startAddrs) {
			if (startAddr.second != startAddrToCompare.second && (startAddrToCompare.second < endAddr  && startAddrToCompare.second >= startAddr.second))
				throw std::runtime_error("Sections overlap, cant run in current mode");
		}
	}
	// no overlaping sections add start addreses of remaining unplaced sections
	for (auto const& sect : sectTable) {
		if (startAddrs.find(sect.first) == startAddrs.end()) {
			startAddrs.insert({ sect.first, highestLocation });
			highestLocation += sect.second.size;
		}
	}

	// check if last section is not above the limit
	if (highestLocation - 1 >= PROGRAM_LIMIT) throw std::runtime_error("program excedes address limit");

	// fix data
	for (auto const& sect : sectTable) {
		if (sect.second.relocationTable) {
			for (auto const& relEntry : *sect.second.relocationTable) {
				Symbol& referencedSymbol = symTable.at(relEntry.symbol);
				int16_t referencedSymbolDisp = startAddrs.at(referencedSymbol.name) - referencedSymbol.value;
				int16_t oldVal = ((uint16_t)data[relEntry.offset + 1]) << 8 | data[relEntry.offset];
				int16_t newVal = 0;
				if (relEntry.type == RelocationEntry::Type::R_386_32) newVal = oldVal + referencedSymbolDisp;
				else if (relEntry.type == RelocationEntry::Type::R_386_N32) newVal = oldVal - referencedSymbolDisp;
				else {
					Symbol& currentSymbol = symTable.at(sect.second.rb);
					int16_t currentSymbolDisp = startAddrs.at(currentSymbol.name) - currentSymbol.value;
					newVal = referencedSymbolDisp - currentSymbolDisp + oldVal;
				}

				data[relEntry.offset] = newVal & 0xFF;
				data[relEntry.offset + 1] = newVal >> 8;

			}
		}
	}

	// move starting point
	programStart += startAddrs.at(".text") - symTable.at(sectTable.at(".text").rb).value; // assuming main is in text for now

	if (init) {
		memWriteWord(programStart, IVCounter);
		systemLocationCounter = highestLocation;
	}

}

void Emulator::keyboardThreadRun(){

	keyboardInterrupt = false;
	while (true) {
		while(keyboardInterrupt){} // wait for the buffer to be emptied
		char tempBuffer;
		std::cin >> std::noskipws >> tempBuffer;
		keyboardBuffer = tempBuffer;
		keyboardInterrupt = true;
	}
}


Emulator::Emulator(){
	
}

Emulator::~Emulator(){
	delete program;
	delete keyboardThread;
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

void Emulator::emulate(std::string filePath, std::unordered_map<std::string, uint16_t>& stAddrs){
	systemInit();

	startAddrs = stAddrs;
	loadProgramFromFile(filePath);
	if (programSize > MAX_PROG_SIZE) throw std::runtime_error("Program too large to be ran");
	program = new Program(*this);
	memWriteWord(programStart,USER_PROGRAM_START_LOCATION);
	program->run();

}
