#include "Program.h"
#include "Emulator.h"
#include "Instruction.h"
#include <string>
#include <regex>

void Program::readInstruction(){
	// get First Byte
	instructionDescr = emulator.memRead(emulator.registers.PC++);
	Instruction::InstrCodes instCode = (Instruction::InstrCodes)(instructionDescr >> 3);
	if (Instruction::instructionNames.find(instCode) == Instruction::instructionNames.end()) {
		// op code not found, not an instruction, stop executing
		running = false;
		// jump to error handling routine int 1
		throw std::runtime_error("Op code not found"); // error until interrupt implementation
	}
	instName = Instruction::instructionNames.at(instCode);
	uint8_t numOfOperands = Instruction::numOfOperands.at(instName);

	bool first = true;
	while (numOfOperands--) {
		uint8_t oprDescr = emulator.memRead(emulator.registers.PC++);
		if (first) opr1Descr = oprDescr;
		else opr2Descr = oprDescr;
		Instruction::AddrCodes addrCode = (Instruction::AddrCodes) (oprDescr >> 5);
		if (addrCode != Instruction::AddrCodes::REGDIR && addrCode != Instruction::AddrCodes::REGIN) {
			if (addrCode == Instruction::AddrCodes::REGINX8 || (addrCode == Instruction::AddrCodes::IMM && !(instructionDescr & 4))) {
				int8_t oprVal = emulator.memRead(emulator.registers.PC++);
				if (first) opr1Val = oprVal;
				else opr2Val = oprVal;
			}
			else {
				uint8_t lowerByte = emulator.memRead(emulator.registers.PC++);
				uint8_t higherByte = emulator.memRead(emulator.registers.PC++);
				int16_t oprVal = ((uint16_t) higherByte << 8) | lowerByte;
				if (first) opr1Val = oprVal;
				else opr2Val = oprVal;
			}
		}
		first = false;
	}
}

int16_t Program::decodeVal(uint8_t oprDescr, int16_t oprVal){
		uint8_t addrCode = oprDescr >> 5;
		uint8_t reg = (oprDescr & 0x1E) >> 1;
		bool word = instructionDescr & 4;
		bool high = oprDescr & 1;
		switch (addrCode) {
		case Instruction::AddrCodes::IMM:
			return oprVal;
		case Instruction::AddrCodes::MEM:
			if (word) return emulator.memReadWord(oprVal);
			else return emulator.memRead(oprVal);
		case Instruction::AddrCodes::REGDIR:
			if(word) return emulator.regRead(reg);
			else return (emulator.regRead(reg) >> (high ? 8 : 0)) & 0xFF;
		case Instruction::AddrCodes::REGIN:
			if(word) return emulator.memReadWord(emulator.regRead(reg));
			else return emulator.memRead(emulator.regRead(reg));
		case Instruction::AddrCodes::REGINX8:
		case Instruction::AddrCodes::REGINX16:
			if(word) return emulator.memReadWord(emulator.regRead(reg) + oprVal);
			else return emulator.memRead(emulator.regRead(reg) + oprVal);
		default:
			throw std::runtime_error("Invalid addresing code");
			return 0; // should not happen
		}
}

size_t Program::decodeAdr(uint8_t oprDescr, int16_t oprVal){
	uint8_t addrCode = oprDescr >> 5;
	uint8_t reg = (oprDescr & 0x1E) >> 1;
	switch (addrCode) {
	case Instruction::AddrCodes::IMM:
		throw std::runtime_error("Immediate addresing cant be used to as a destination operand");
	case Instruction::AddrCodes::MEM:
		return oprVal;
	case Instruction::AddrCodes::REGDIR:
		return reg;
	case Instruction::AddrCodes::REGIN:
		return emulator.regRead(reg);
	case Instruction::AddrCodes::REGINX8:
	case Instruction::AddrCodes::REGINX16:
		return emulator.regRead(reg) + oprVal;
	default:
		throw std::runtime_error("Invalid addresing code");
		return 0; // should not happen
	}
}



void Program::execute(){
	Instruction::InstrCodes instCode = (Instruction::InstrCodes) Instruction::instructionCodes.at(instName);
	switch (instCode){
	case Instruction::HALT:
		HALT_EXECUTOR();
		break;
	case Instruction::XCHG:
		XCHG_EXECUTOR();
		break;
	case Instruction::INT:
		INT_EXECUTOR();
		break;
	case Instruction::MOV:
		MOV_EXECUTOR();
		break;
	case Instruction::ADD:
		ADD_EXECUTOR();
		break;
	case Instruction::SUB:
		SUB_EXECUTOR();
		break;
	case Instruction::MUL:
		MUL_EXECUTOR();
		break;
	case Instruction::DIV:
		DIV_EXECUTOR();
		break;
	case Instruction::CMP:
		CMP_EXECUTOR();
		break;
	case Instruction::NOT:
		NOT_EXECUTOR();
		break;
	case Instruction::AND:
		AND_EXECUTOR();
		break;
	case Instruction::OR:
		OR_EXECUTOR();
		break;
	case Instruction::XOR:
		XOR_EXECUTOR();
		break;
	case Instruction::TEST:
		TEST_EXECUTOR();
		break;
	case Instruction::SHL:
		SHL_EXECUTOR();
		break;
	case Instruction::SHR:
		SHR_EXECUTOR();
		break;
	case Instruction::PUSH:
		PUSH_EXECUTOR();
		break;
	case Instruction::POP:
		POP_EXECUTOR();
		break;
	case Instruction::JMP:
		JMP_EXECUTOR();
		break;
	case Instruction::JEQ:
		JEQ_EXECUTOR();
		break;
	case Instruction::JNE:
		JNE_EXECUTOR();
		break;
	case Instruction::JGT:
		JGT_EXECUTOR();
		break;
	case Instruction::CALL:
		CALL_EXECUTOR();
		break;
	case Instruction::RET:
		RET_EXECUTOR();
		break;
	case Instruction::IRET:
		IRET_EXECUTOR();
		break;
	default:
		break;
	}
}




void Program::run() {
	running = true;
	while (running) {
		readInstruction();
		execute();
	}
}

void Program::HALT_EXECUTOR(){
	running = false;
}

void Program::XCHG_EXECUTOR(){
	int16_t val1 = decodeVal(opr1Descr, opr1Val);
	int16_t val2 = decodeVal(opr2Descr, opr2Val);
	size_t adr1 = decodeAdr(opr1Descr, opr1Val);
	size_t adr2 = decodeAdr(opr2Descr, opr2Descr);
	bool word = instructionDescr & 4;

	if (opr1Descr >> 5 == Instruction::AddrCodes::REGDIR) emulator.regWrite(val2, adr1);
	else {
		if (word) emulator.memWriteWord(val2, adr1);
		else emulator.memWrite(val2, adr1);
	}
	if (opr2Descr >> 5 == Instruction::AddrCodes::REGDIR) emulator.regWrite(val1, adr2);
	else {
		if (word) emulator.memWriteWord(val1, adr2);
		else emulator.memWrite(val1, adr2);
	}
}

void Program::INT_EXECUTOR(){
	emulator.stackPushWord(emulator.PSW.val);
	emulator.registers.PC = emulator.memory[decodeVal(opr1Descr, opr1Val) % 8 * 2];
}

void Program::MOV_EXECUTOR(){
	int16_t val = decodeVal(opr2Descr, opr2Val);
	size_t adr = decodeAdr(opr1Descr, opr1Val);
	if (opr1Descr >> 5 == Instruction::AddrCodes::REGDIR) emulator.regWrite(val, adr);
	else 
		if (instructionDescr & 4) emulator.memWriteWord(val, adr);
		else emulator.memWrite(val, adr);
}

void Program::SUB_EXECUTOR(){
	int16_t val1 = decodeVal(opr1Descr, opr1Val);
	int16_t val2 = decodeVal(opr2Descr, opr2Val);
	uint8_t reg = decodeAdr(opr1Descr, opr1Val);
	emulator.regWrite(val1 - val2, reg);
}

void Program::ADD_EXECUTOR(){
	int16_t val1 = decodeVal(opr1Descr, opr1Val);
	int16_t val2 = decodeVal(opr2Descr, opr2Val);
	uint8_t reg = decodeAdr(opr1Descr, opr1Val);
	emulator.regWrite(val1 + val2, reg);
}

void Program::MUL_EXECUTOR(){
	int16_t val1 = decodeVal(opr1Descr, opr1Val);
	int16_t val2 = decodeVal(opr2Descr, opr2Val);
	uint8_t reg = decodeAdr(opr1Descr, opr1Val);
	emulator.regWrite(val1 * val2, reg);
}

void Program::DIV_EXECUTOR(){
	int16_t val1 = decodeVal(opr1Descr, opr1Val);
	int16_t val2 = decodeVal(opr2Descr, opr2Val);
	uint8_t reg = decodeAdr(opr1Descr, opr1Val);
	emulator.regWrite(val1 / val2, reg);
}

void Program::CMP_EXECUTOR(){

}

void Program::NOT_EXECUTOR(){

}

void Program::AND_EXECUTOR(){
	uint16_t val1 = decodeVal(opr1Descr, opr1Val);
	uint16_t val2 = decodeVal(opr2Descr, opr2Val);
	uint8_t reg = decodeAdr(opr1Descr, opr1Val);
	emulator.regWrite(val1 & val2, reg);
}

void Program::OR_EXECUTOR(){
	uint16_t val1 = decodeVal(opr1Descr, opr1Val);
	uint16_t val2 = decodeVal(opr2Descr, opr2Val);
	uint8_t reg = decodeAdr(opr1Descr, opr1Val);
	emulator.regWrite(val1 | val2, reg);
}

void Program::XOR_EXECUTOR(){
	uint16_t val1 = decodeVal(opr1Descr, opr1Val);
	uint16_t val2 = decodeVal(opr2Descr, opr2Val);
	uint8_t reg = decodeAdr(opr1Descr, opr1Val);
	emulator.regWrite(val1 ^ val2, reg);
}

void Program::TEST_EXECUTOR(){

}

void Program::SHL_EXECUTOR()
{
}

void Program::SHR_EXECUTOR()
{
}

void Program::PUSH_EXECUTOR()
{
}

void Program::POP_EXECUTOR()
{
}

void Program::JMP_EXECUTOR()
{
}

void Program::JEQ_EXECUTOR()
{
}

void Program::JNE_EXECUTOR()
{
}

void Program::JGT_EXECUTOR()
{
}

void Program::CALL_EXECUTOR()
{
}

void Program::RET_EXECUTOR()
{
}

void Program::IRET_EXECUTOR()
{
}
