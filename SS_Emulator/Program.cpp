#include "Program.h"
#include "Emulator.h"
#include "Instruction.h"
#include <string>

void Program::fetch(){
	// get First Byte
	instructionDescr = emulator.memRead(emulator.registers.PC++);
	Instruction::InstrCodes instCode = (Instruction::InstrCodes)(instructionDescr >> 3);
	if (Instruction::instructionNames.find(instCode) == Instruction::instructionNames.end()) {
		// op code not found, not an instruction, stop executing
		running = false;
		// jump to error handling routine int 1
		throw std::runtime_error("op code not found"); // error until interrupt implementation
	}
	std::string instName = Instruction::instructionNames.at(instCode);
	uint8_t numOfOperands = Instruction::numOfOperands.at(instName);

	while (numOfOperands--) {
		uint8_t oprDescr = emulator.memRead(emulator.registers.PC++);
		if (numOfOperands) opr1Descr = oprDescr;
		else opr2Descr = oprDescr;
		Instruction::AddrCodes addrCode = (Instruction::AddrCodes) (oprDescr >> 5);
		if (addrCode != Instruction::AddrCodes::REGDIR && addrCode != Instruction::AddrCodes::REGIN) {
			if (addrCode == Instruction::AddrCodes::REGINX8 || (addrCode == Instruction::AddrCodes::IMM && !(oprDescr & 4))) {
				int8_t oprVal = emulator.memRead(emulator.registers.PC++);
				if (numOfOperands) opr1Val = oprVal;
				else opr2Val = oprVal;
			}
			else {
				uint8_t lowerByte = emulator.memRead(emulator.registers.PC++);
				uint8_t higherByte = emulator.memRead(emulator.registers.PC++);
				int16_t oprVal = ((uint16_t) higherByte << 8) | lowerByte;
				if (numOfOperands) opr1Val = oprVal;
				else opr2Val = oprVal;
			}
		}
	}
}

void Program::decode(){

}

void Program::execute(){

}

void Program::run() {
	running = true;
	while (running) {
		fetch();
		decode();
		execute();
	}
}
