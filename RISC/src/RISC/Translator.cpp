#include "RISC/Translator.h"
#include <bitset>
#include <iostream>
#include <tuple>
#include <map>

#define IF_ALL_EQ_DO(n, func, ...)                         \
	{                                                      \
		bool isEqual = false;                              \
		std::string arr[] = {__VA_ARGS__};                 \
		uint32_t size = sizeof(arr) / sizeof(std::string); \
		for (uint32_t i = 0; i < size; i++)                \
			if (arr[i] == n)                               \
			{                                              \
				isEqual = true;                            \
				break;                                     \
			}                                              \
		if (isEqual)                                       \
			func;                                          \
	}

namespace RISC
{
	inline static uint32_t PutBitsInUint(uint32_t &stream, uint32_t bitsStream, uint32_t from, uint32_t to)
	{
		uint32_t mask = (((0xffffffff << (32u - (to + 1))) >> (32 - (to + 1))) >> from) << from;
		uint32_t maskInvert = ~mask;
		stream &= maskInvert;					 // clearing the bits of the mask
		stream |= (mask & (bitsStream << from)); // getting the target bits from the bitsStream and setting them in the stream
		return stream;
	}

	inline static uint32_t ExtractBitsFromUint(const uint32_t &stream, uint32_t from, uint32_t to)
	{
		uint32_t mask = (((0xffffffff << (32u - (to + 1))) >> (32 - (to + 1))) >> from) << from;
		uint32_t output = stream & mask; // clearing the bits of the mask
		return output;
	}

	struct Space
	{
		bool valid : 1;
		int8_t from : 5;
		int8_t to : 5;
	};

	/*
	opcode, rd, rs1, rs2, funct3, funct7, imm
	*/
	struct Pattern
	{
		Space opcode;
		Space rd;
		Space rs1;
		Space rs2;
		Space funct3;
		Space funct7;
		Space imm;
	};

	struct Format
	{
		uint8_t opcode : 7;
		uint8_t rd : 5;
		uint8_t rs1 : 5;
		uint8_t rs2 : 5;
		uint8_t funct3 : 3;
		uint8_t funct7 : 7;
		uint32_t imm : 20;
	};

	inline static uint32_t AssembleType(const Format &frmt, const Pattern &pattern)
	{
		uint32_t output = 0;

		if (pattern.opcode.valid)
			PutBitsInUint(output, frmt.opcode, pattern.opcode.from, pattern.opcode.to);
		if (pattern.rs1.valid)
			PutBitsInUint(output, frmt.rs1, pattern.rs1.from, pattern.rs1.to);
		if (pattern.rs2.valid)
			PutBitsInUint(output, frmt.rs2, pattern.rs2.from, pattern.rs2.to);
		if (pattern.rd.valid)
			PutBitsInUint(output, frmt.rd, pattern.rd.from, pattern.rd.to);
		if (pattern.funct3.valid)
			PutBitsInUint(output, frmt.funct3, pattern.funct3.from, pattern.funct3.to);
		if (pattern.funct7.valid)
			PutBitsInUint(output, frmt.funct7, pattern.funct7.from, pattern.funct7.to);
		if (pattern.imm.valid)
			PutBitsInUint(output, frmt.imm, pattern.imm.from, pattern.imm.to);

		return output;
	}

	static const std::map<std::string, uint32_t> g_InstToFunc3 = {
		{"BEQ", 0b000},
		{"BNE", 0b001},
		{"BLT", 0b100},
		{"BGE", 0b101},
		{"BLTU", 0b110},
		{"BGEU", 0b111},
		{"LB", 0},
		{"LH", 1},
		{"LW", 2},
		{"LBU", 4},
		{"LHU", 5},
		{"SB", 0},
		{"SH", 1},
		{"SW", 2},
		{"ADDI", 0},
		{"SLTI", 2},
		{"SLTIU", 3},
		{"XORI", 4},
		{"ORI", 0b110},
		{"ANDI", 0b111},
		{"SLLI", 0b001},
		{"SRLI", 0b101},
		{"ADD", 0},
		{"SUB", 0},
		{"SLL", 1},
		{"SLT", 2},
		{"SLTU", 3},
		{"XOR", 4},
		{"SRL", 5},
		{"SRA", 5},
		{"OR", 6},
		{"AND", 7},
		{"ECALL", 0},
		{"EBREAK", 0}};

	static const std::map<std::string, uint32_t> g_InstToFunc7 = {
		{"ADD", 0},
		{"SUB", 0b0100000},
		{"SLL", 0},
		{"SLT", 0},
		{"SLTU", 0},
		{"XOR", 0},
		{"SRL", 0},
		{"SRA", 0b0100000},
		{"OR", 0},
		{"AND", 0}};

	static const Pattern rType{
		{true, 0, 6},
		{true, 7, 11},
		{true, 15, 19},
		{true, 20, 24},
		{true, 12, 14},
		{true, 25, 31},
		{false, -1, -1}};
	// opcode, rd, rs1, rs2, funct3, funct7, imm
	static const Pattern iType{
		{true, 0, 6},
		{true, 7, 11},
		{true, 15, 19},
		{false, -1, -1},
		{true, 12, 14},
		{false, -1, -1},
		{true, 20, 31}};
	static const Pattern sType{
		{true, 0, 6},
		{true, 7, 11},
		{true, 15, 19},
		{true, 20, 24},
		{true, 12, 14},
		{true, 25, 31},
		{false, -1, -1}};
	static const Pattern uType{
		{true, 0, 6},
		{true, 7, 11},
		{false, 15, 19},
		{false, 20, 24},
		{false, 12, 14},
		{false, 25, 31},
		{true, 12, 31}};
	static const Pattern bType{
		{true, 0, 6},
		{true, 7, 11},
		{true, 15, 19},
		{true, 20, 24},
		{true, 12, 14},
		{true, 25, 31},
		{false, -1, -1}};
	static const Pattern jType{
		{true, 0, 6},
		{true, 7, 11},
		{false, 15, 19},
		{false, 20, 24},
		{false, 12, 14},
		{false, 25, 31},
		{true, 12, 31}};

	inline static uint32_t ConstructBImm(uint32_t imm)
	{
		uint32_t bit11 = ExtractBitsFromUint(imm, 11, 11);
		uint32_t bits1_4 = ExtractBitsFromUint(imm, 1, 4);
		uint32_t bits5_10 = ExtractBitsFromUint(imm, 5, 10);
		uint32_t bit12 = ExtractBitsFromUint(imm, 12, 12);
		//[d][c][b][a]
		// a => bit11
		// b => bits1_4
		// c => bits5_10
		// d => bit12
		uint32_t output = (bit11 >> 11) | (bits1_4) | bits5_10 | bit12 >> 1;
		return output;
	}

	inline static uint32_t ConstructJImm(uint32_t imm)
	{
		uint32_t bits12_19 = ExtractBitsFromUint(imm, 12, 19);
		uint32_t bit11 = ExtractBitsFromUint(imm, 11, 11);
		uint32_t bits1_10 = ExtractBitsFromUint(imm, 1, 10);
		uint32_t bit20 = ExtractBitsFromUint(imm, 20, 20);
		//[d][c][b][a]
		// a => bit12_19
		// b => bit11
		// c => bits1_10
		// d => bit20
		uint32_t output = (bits12_19 >> 12) | (bit11 >> 3) | (bits1_10 << 8) | bit20;
		return output;
	}

	inline static void AssembleRType(Instruction &instruction, uint32_t &output)
	{
		Format rInstruction;
		rInstruction.opcode = 0b0110011;
		rInstruction.funct3 = g_InstToFunc3.at(instruction.instName);
		rInstruction.funct7 = g_InstToFunc7.at(instruction.instName);
		rInstruction.rs1 = instruction.rs1;
		rInstruction.rs2 = instruction.rs2;
		rInstruction.rd = instruction.rd;

		output = AssembleType(rInstruction, rType);
	}

	inline static void AssembleIType(Instruction &instruction, uint32_t &output)
	{
		Format iInstruction;
		iInstruction.opcode = 0b0010011;
		iInstruction.funct3 = g_InstToFunc3.at(instruction.instName);
		iInstruction.rs1 = instruction.rs1;
		iInstruction.imm = instruction.imm;
		if (instruction.instName == "SRAI")
			iInstruction.imm |= (0b0100000 << 5);
		iInstruction.rd = instruction.rd;

		output = AssembleType(iInstruction, iType);
	}

	inline static void AssembleSType(Instruction &instruction, uint32_t &output)
	{
		Format sInstruction;
		sInstruction.funct7 = instruction.imm >> 5;
		sInstruction.rd = instruction.imm & 0x1f;
		sInstruction.rs1 = instruction.rs1;
		sInstruction.rs2 = instruction.rs2;
		sInstruction.opcode = 0b0100011;
		sInstruction.funct3 = g_InstToFunc3.at(instruction.instName);

		output = AssembleType(sInstruction, sType);
	}

	inline static void AssembleBType(Instruction &instruction, uint32_t &output)
	{
		Format bInstruction;
		uint32_t imm = ConstructBImm(instruction.imm);
		bInstruction.funct7 = imm >> 5;
		bInstruction.rd = imm & 0x1f;
		bInstruction.rs1 = instruction.rs1;
		bInstruction.rs2 = instruction.rs2;
		bInstruction.opcode = 0b1100011;
		bInstruction.funct3 = g_InstToFunc3.at(instruction.instName);

		output = AssembleType(bInstruction, bType);
	}

	inline static void AssembleUType(Instruction &instruction, uint32_t &output)
	{
		Format uInstruction;
		if (instruction.instName == "LUI")
			uInstruction.opcode = 0b0110111;
		else if (instruction.instName == "AUIPC")
			uInstruction.opcode = 0b0010111;
		uInstruction.rd = instruction.rd;
		uInstruction.imm = instruction.imm;
		output = AssembleType(uInstruction, uType);
	}

	inline static void AssembleJType(Instruction &instruction, uint32_t &output)
	{
		Format jInstruction;
		jInstruction.rd = instruction.rd;
		jInstruction.imm = ConstructJImm(instruction.imm);
		jInstruction.opcode = 0b1101111;

		output = AssembleType(jInstruction, jType);
	}

	inline static void AssembleEnvType(Instruction &instruction, uint32_t &output)
	{
		if (instruction.instName == "ECALL")
			output = 0x00000073;
		else if (instruction.instName == "EBREAK")
			output = 0x00100073;
	}

	uint32_t Translator::Assemble(Instruction &instruction)
	{
		const auto &n = instruction.instName;
		std::cout << "Name: " << n << std::endl;
		uint32_t output = 0;
		IF_ALL_EQ_DO(
			n, { AssembleUType(instruction, output); return output; }, "LUI", "AUIPC");
		IF_ALL_EQ_DO(
			n, { AssembleRType(instruction, output); return output; }, "ADD", "SUB", "SLL", "SLT", "SLTU", "XOR", "SRL", "SRA", "OR", "AND");
		IF_ALL_EQ_DO(
			n, { AssembleIType(instruction, output); return output; }, "ADDI", "SLLI", "SLTI", "SLTIU", "XORI", "SRLI", "SRAI", "ORI", "ANDI", "LW", "LB", "LH", "LBU", "LHU", "JALR");
		IF_ALL_EQ_DO(
			n, { AssembleBType(instruction, output); return output; }, "BEQ", "BNE", "BLT", "BGE", "BLTU", "BGEU");
		IF_ALL_EQ_DO(
			n, { AssembleJType(instruction, output); return output; }, "JAL");
		IF_ALL_EQ_DO(
			n, { AssembleSType(instruction, output); return output; }, "SB", "SH", "SW");
		IF_ALL_EQ_DO(
			n, { AssembleEnvType(instruction, output); return output; }, "ECALL", "EBREAK");
		return output;
	}

	Instruction Translator::Dissassemble(uint32_t instruction)
	{
	}
}