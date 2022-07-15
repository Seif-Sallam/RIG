#include "RISC/Instruction.h"
#include <stdlib.h>
#include <unordered_map>
#include <algorithm>
#include <cstring>

#define TYPE_NAME(funcName) #funcName
#define TABLE_ENTRY(funcName)              \
	{                                      \
		TYPE_NAME(funcName), funcName##_32 \
	}

namespace RISC
{
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
	inline static bool ECALL_32(Instruction &self) { return true; }
	inline static bool EBREAK_32(Instruction &self) { return true; }

	const std::vector<const char *>
		Instruction::typeNames = {
			"ADD",
			"SUB",
			"XOR",
			"OR",
			"AND",
			"SLL",
			"SRL",
			"SLT",
			"SLTU",
			"ADDI",
			"XORI",
			"ORI",
			"ANDI",
			"SLLI",
			"SRLI",
			"SRAI",
			"SLTI",
			"SLTIU",
			"LB",
			"LH",
			"LW",
			"LBU",
			"LHU",
			"SB",
			"SH",
			"SW",
			"BEQ",
			"BNE",
			"BLT",
			"BGE",
			"BLTU",
			"BGEU",
			"JAL",
			"JALR",
			"LUI",
			"AUIPC",
			"ECALL",
			"EBREAK"};

	static const std::unordered_map<const char *, bool (*)(Instruction &)> operationsTable{
		TABLE_ENTRY(ADD),
		TABLE_ENTRY(SUB),
		TABLE_ENTRY(XOR),
		TABLE_ENTRY(OR),
		TABLE_ENTRY(AND),
		TABLE_ENTRY(SLL),
		TABLE_ENTRY(SRL),
		TABLE_ENTRY(SLT),
		TABLE_ENTRY(SLTU),
		TABLE_ENTRY(ADDI),
		TABLE_ENTRY(XORI),
		TABLE_ENTRY(ORI),
		TABLE_ENTRY(ANDI),
		TABLE_ENTRY(SLLI),
		TABLE_ENTRY(SRLI),
		TABLE_ENTRY(SRAI),
		TABLE_ENTRY(SLTI),
		TABLE_ENTRY(SLTIU),
		TABLE_ENTRY(LB),
		TABLE_ENTRY(LH),
		TABLE_ENTRY(LW),
		TABLE_ENTRY(LBU),
		TABLE_ENTRY(LHU),
		TABLE_ENTRY(SB),
		TABLE_ENTRY(SH),
		TABLE_ENTRY(SW),
		TABLE_ENTRY(BEQ),
		TABLE_ENTRY(BNE),
		TABLE_ENTRY(BLT),
		TABLE_ENTRY(BGE),
		TABLE_ENTRY(BLTU),
		TABLE_ENTRY(BGEU),
		TABLE_ENTRY(JAL),
		TABLE_ENTRY(JALR),
		TABLE_ENTRY(LUI),
		TABLE_ENTRY(AUIPC),
		TABLE_ENTRY(ECALL),
		TABLE_ENTRY(EBREAK),
	};

	Instruction::Instruction(const std::string &type)
		: rs1(0), rs2(0), rd(0), imm(0), instName(type)
	{
		Init(type);
	}

	void Instruction::Init(const std::string &type)
	{
		this->instName = type;
		for (auto &c : this->instName)
			c = toupper(c);

		if (auto it = operationsTable.find(instName.c_str()); it != operationsTable.end())
			m_Operation = it->second;
		else
			m_Operation = nullptr;
		m_Initialized = true;

		WriteType();
	}

	bool Instruction::Operate()
	{
		if (m_Operation == nullptr)
			return false;
		return m_Operation(*this);
	}

	static const std::vector<std::string> g_RType = {
		"ADD",
		"SUB",
		"SLL",
		"SLT",
		"SLTU",
		"XOR",
		"AND",
		"OR",
		"SRL",
		"SRA",
	};

	static const std::vector<std::string> g_IType = {
		"ADDI",
		"SUBI",
		"SLLI",
		"SLTI",
		"SLTIU",
		"XORI",
		"ANDI",
		"ORI",
		"SRLI",
		"SRAI",
		"AUIPC",
		"LUI",
	};

	static const std::vector<std::string> g_BType = {
		"BEQ",
		"BNE",
		"BLT",
		"BGE",
		"BLTU",
		"BGEU",
	};

	static const std::vector<std::string> g_LType = {
		"LW",
		"LH",
		"LB",
		"LHU",
		"LBU",
	};

	static const std::vector<std::string> g_SType = {
		"SW",
		"SH",
		"SB",
	};

	static const std::vector<std::string> g_JType = {
		"JALR",
		"JAL",
	};

	static const std::vector<std::string> g_EnvType = {
		"ECALL",
		"EBREAK",
	};

	void Instruction::WriteType()
	{
		auto &name = this->instName;
		if (name == "LUI" || name == "AUIPC" || name == "JAL")
			writeType = TYPE1;
		else if (name == "JALR" || std::find(g_LType.begin(), g_LType.end(), name) != g_LType.end())
			writeType = TYPE2;
		else if (std::find(g_SType.begin(), g_SType.end(), name) != g_SType.end())
			writeType = TYPE3;
		else if (std::find(g_RType.begin(), g_RType.end(), name) != g_RType.end())
			writeType = TYPE4;
		else if (std::find(g_IType.begin(), g_IType.end(), name) != g_IType.end())
			writeType = TYPE5;
		else if (std::find(g_EnvType.begin(), g_EnvType.end(), name) != g_EnvType.end())
			writeType = TYPE6;
		else if (std::find(g_BType.begin(), g_BType.end(), name) != g_BType.end())
			writeType = TYPE7;
		else
			writeType = INVALID;
	}

	const std::string Instruction::FormatInstruction(Instruction &inst)
	{
		std::string output = inst.instName;
		const InstWritingType type = inst.writeType;
		char format[1024];
		memset(format, 0, 1024);
		int n = 0;

		switch (type)
		{
		case TYPE1:
			n = sprintf(format, " x%u, %d", inst.rd, inst.imm);
			break;
		case TYPE2:
			n = sprintf(format, " x%u, %d(x%u)", inst.rd, inst.imm, inst.rs1);
			break;
		case TYPE3:
			n = sprintf(format, " x%u, %d(x%u)", inst.rs2, inst.imm, inst.rs1);
			break;
		case TYPE4:
			n = sprintf(format, " x%u, x%u, x%u", inst.rd, inst.rs1, inst.rs2);
			break;
		case TYPE5:
			n = sprintf(format, " x%u, x%u, %d", inst.rd, inst.rs1, inst.imm);
			break;
		case TYPE6:
			break;
		case TYPE7:
			n = sprintf(format, " x%u, x%u, %d", inst.rs1, inst.rs2, inst.imm);
			break;
		default:
			n = sprintf(format, "<Unspported Instruction>");
			break;
		}

		output += std::string(format);
		return output;
	}
}