#include "Rig/Parser.h"
#include <unordered_map>
#include <array>

#define TYPE_NAME(funcName) #funcName
#define TABLE_ENTRY(funcName)         \
	{                                 \
		TYPE_NAME(funcName), funcName \
	}

namespace Rig
{
	namespace Asm
	{
		// const Instruction Instruction::NOP("addi", 0);
		inline static uint32_t FromValuesToUint(uint8_t rs1, uint8_t rs2, uint8_t rd, uint16_t imm)
		{
			uint32_t output = 0;
			output = ((uint32_t)rs1 << 0) | ((uint32_t)rs2 << 5) | ((uint32_t)rd << 10) | ((uint32_t)imm << 15);

			return output;
		}

		inline static void FromUintToValues(uint32_t value, uint8_t &rs1, uint8_t &rs2, uint8_t rd, uint16_t &imm)
		{
			rs1 = value & 0x01f;
			rs2 = (value >> 5) & 0x01f;
			rd = (value >> 10) & 0x01f;
			imm = (value >> 15);
		}

		inline static std::string SanitizeString(std::string str)
		{
			bool foundWhiteSpace = false;
			for (uint32_t i = 0; i < str.size(); i++)
			{
				if (str[i] == ':')
				{
					int index = i - 1;
					while (index >= 0)
					{
						char c = str[index--];
						if (c == char(27) || c == ' ')
						{
							if (c == ' ')
							{
								str[index + 1] = char(27);
								break;
							}
						}
						else
							break;
					}
				}
				if (str[i] == ' ')
				{
					if (foundWhiteSpace)
						str[i] = char(27); // A character that most probably won't exist.
					foundWhiteSpace = true;
				}
				else
					foundWhiteSpace = false;
			}
			auto it = std::remove_if(str.begin(), str.end(), [](const char &c)
									 { return c == (char)27; });

			str.erase(it, str.end());
			return str;
		}

		static const std::unordered_map<const char *, bool (*)(Instruction &)> operationsTable{
			TABLE_ENTRY(ADD_32),
			TABLE_ENTRY(SUB_32),
			TABLE_ENTRY(XOR_32),
			TABLE_ENTRY(OR_32),
			TABLE_ENTRY(AND_32),
			TABLE_ENTRY(SLL_32),
			TABLE_ENTRY(SRL_32),
			TABLE_ENTRY(SLT_32),
			TABLE_ENTRY(SLTU_32),
			TABLE_ENTRY(ADDI_32),
			TABLE_ENTRY(XORI_32),
			TABLE_ENTRY(ORI_32),
			TABLE_ENTRY(ANDI_32),
			TABLE_ENTRY(SLLI_32),
			TABLE_ENTRY(SRLI_32),
			TABLE_ENTRY(SRAI_32),
			TABLE_ENTRY(SLTI_32),
			TABLE_ENTRY(SLTIU_32),
			TABLE_ENTRY(LB_32),
			TABLE_ENTRY(LH_32),
			TABLE_ENTRY(LW_32),
			TABLE_ENTRY(LBU_32),
			TABLE_ENTRY(LHU_32),
			TABLE_ENTRY(SB_32),
			TABLE_ENTRY(SH_32),
			TABLE_ENTRY(SW_32),
			TABLE_ENTRY(BEQ_32),
			TABLE_ENTRY(BNE_32),
			TABLE_ENTRY(BLT_32),
			TABLE_ENTRY(BGE_32),
			TABLE_ENTRY(BLTU_32),
			TABLE_ENTRY(BGEU_32),
			TABLE_ENTRY(JAL_32),
			TABLE_ENTRY(JALR_32),
			TABLE_ENTRY(LUI_32),
			TABLE_ENTRY(AUIPC_32),
		};

		static const std::vector<const char *> typeNames = {
			TYPE_NAME(ADD_32),
			TYPE_NAME(SUB_32),
			TYPE_NAME(XOR_32),
			TYPE_NAME(OR_32),
			TYPE_NAME(AND_32),
			TYPE_NAME(SLL_32),
			TYPE_NAME(SRL_32),
			TYPE_NAME(SLT_32),
			TYPE_NAME(SLTU_32),
			TYPE_NAME(ADDI_32),
			TYPE_NAME(XORI_32),
			TYPE_NAME(ORI_32),
			TYPE_NAME(ANDI_32),
			TYPE_NAME(SLLI_32),
			TYPE_NAME(SRLI_32),
			TYPE_NAME(SRAI_32),
			TYPE_NAME(SLTI_32),
			TYPE_NAME(SLTIU_32),
			TYPE_NAME(LB_32),
			TYPE_NAME(LH_32),
			TYPE_NAME(LW_32),
			TYPE_NAME(LBU_32),
			TYPE_NAME(LHU_32),
			TYPE_NAME(SB_32),
			TYPE_NAME(SH_32),
			TYPE_NAME(SW_32),
			TYPE_NAME(BEQ_32),
			TYPE_NAME(BNE_32),
			TYPE_NAME(BLT_32),
			TYPE_NAME(BGE_32),
			TYPE_NAME(BLTU_32),
			TYPE_NAME(BGEU_32),
			TYPE_NAME(JAL_32),
			TYPE_NAME(JALR_32),
			TYPE_NAME(LUI_32),
			TYPE_NAME(AUIPC_32),
		};

		Instruction::Instruction()
		{
		}
		void Instruction::Init(const char *instructionName, uint32_t values)
		{
			m_Operation = operationsTable.at(instructionName);
			FromUintToValues(values, rs1, rs2, rd, imm);
		}

		bool Instruction::Operate()
		{
			return m_Operation(*this);
		}

		Parser::Parser(std::istream &inputStream)
			: m_InputStream(inputStream)
		{
			m_LabelsMap.clear();
		}

		struct LineData
		{
			bool error = false;
			uint32_t colonIndex = 0;
			uint32_t hashIndex = 0;
			uint32_t instructionStart = 0;
			inline operator bool() const
			{
				return error;
			}
		};

		LineData GetLineInfo(const std::string &str)
		{
			LineData data;
			uint32_t numOfColons = 0;

			for (uint32_t i = 0; i < str.size(); i++)
			{
				if (str[i] == ':')
				{
					numOfColons++;
					if (i + 2 < str.size())
					{
						if (str[i + 1] == ' ')
							data.instructionStart = i + 2;
						else
							data.instructionStart = i + 1;
					}
					data.colonIndex = i;
				}
				if (str[i] == '#')
				{
					data.hashIndex = i;
				}
			}
			if (numOfColons > 0)
				data.error = true;

			return data;
		}

		Instruction Parser::ReadNextLine()
		{
			bool instructionRead = false;
			uint32_t counter = 0;
			Instruction inst{}; // NOP
			while (!instructionRead && !m_InputStream.eof())
			{
				counter++;
				std::string line;
				std::getline(m_InputStream, line);
				if (line.size() < 2)
					continue;
				auto info = GetLineInfo(line);
				if (info)
				{
					std::cerr << "Too many colons in instruciton line: " << counter << "\n\t" << line;
					break;
				}

				if (auto it = std::find(typeNames.begin(), typeNames.end(), line.c_str()); it != typeNames.end())
				{
					// Get the values form the instruction using a parsing function
					inst.Init(*it, 0);
				}
			}
			return inst;
		}

		Instruction Parser::ParseInstruction(const std::string &instructionLine)
		{
			return Instruction{};
		}
	}
}