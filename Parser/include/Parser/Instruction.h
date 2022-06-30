#pragma once
#include <string>
#include <vector>

struct Instruction
{
	std::string line;
	uint32_t rs1, rs2, rd, imm;
	static const std::vector<const char *> typeNames;
};

inline static bool ADD_32(Instruction &self) { return true; }
inline static bool SUB_32(Instruction &self) { return true; }
inline static bool XOR_32(Instruction &self) { return true; }
inline static bool OR_32(Instruction &self) { return true; }
inline static bool AND_32(Instruction &self) { return true; }
inline static bool SLL_32(Instruction &self) { return true; }
inline static bool SRL_32(Instruction &self) { return true; }
inline static bool SLT_32(Instruction &self) { return true; }
inline static bool SLTU_32(Instruction &self) { return true; }
inline static bool ADDI_32(Instruction &self) { return true; }
inline static bool XORI_32(Instruction &self) { return true; }
inline static bool ORI_32(Instruction &self) { return true; }
inline static bool ANDI_32(Instruction &self) { return true; }
inline static bool SLLI_32(Instruction &self) { return true; }
inline static bool SRLI_32(Instruction &self) { return true; }
inline static bool SRAI_32(Instruction &self) { return true; }
inline static bool SLTI_32(Instruction &self) { return true; }
inline static bool SLTIU_32(Instruction &self) { return true; }
inline static bool LB_32(Instruction &self) { return true; }
inline static bool LH_32(Instruction &self) { return true; }
inline static bool LW_32(Instruction &self) { return true; }
inline static bool LBU_32(Instruction &self) { return true; }
inline static bool LHU_32(Instruction &self) { return true; }
inline static bool SB_32(Instruction &self) { return true; }
inline static bool SH_32(Instruction &self) { return true; }
inline static bool SW_32(Instruction &self) { return true; }
inline static bool BEQ_32(Instruction &self) { return true; }
inline static bool BNE_32(Instruction &self) { return true; }
inline static bool BLT_32(Instruction &self) { return true; }
inline static bool BGE_32(Instruction &self) { return true; }
inline static bool BLTU_32(Instruction &self) { return true; }
inline static bool BGEU_32(Instruction &self) { return true; }
inline static bool JAL_32(Instruction &self) { return true; }
inline static bool JALR_32(Instruction &self) { return true; }
inline static bool LUI_32(Instruction &self) { return true; }
inline static bool AUIPC_32(Instruction &self) { return true; }