#pragma once

#include "Instruction.h"

// From pesudo instructions to real instructions
// Dissassembly
// Assembly
namespace RISC
{
	class Translator
	{
	public:
		// static std::vector<Instruction> FromPesudoToInstrucitons(Instruction &instruction);
		static uint32_t Assemble(Instruction &instruction);
		static Instruction Dissassemble(uint32_t instruction);
	};

}