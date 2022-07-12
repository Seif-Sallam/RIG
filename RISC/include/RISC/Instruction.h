#pragma once
#include <string>
#include <vector>

namespace RISC
{
	enum InstType
	{
		TYPE1, // inst rd, imm
		TYPE2, // inst rd, imm(rs1)
		TYPE3, // inst rs2, imm(rs1)
		TYPE4, // inst rd, rs1, rs2
		TYPE5, // inst rd, rs1, imm
		TYPE6, // ecall / fence / ebreak
		TYPE7, // inst rs1, rs2, imm
		INVALID
	};

	class Instruction
	{
	public:
		Instruction(const std::string &instType);
		bool Operate();
		static const std::vector<const char *> typeNames;

		uint32_t rs1, rs2, rd, imm;
		std::string instType;
		uint32_t lineNumber = 0;
		uint32_t address;
		InstType WriteType();
		static const std::string FormatInstruction(Instruction &inst);

	private:
		bool (*m_Operation)(Instruction &);
	};
}