#pragma once
#include <iostream>
#include <istream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unordered_map>
namespace Rig
{
	namespace Asm
	{
		struct Instruction
		{
		public:
			Instruction();
			void Init(const char *instructionName, uint32_t values);
			bool Operate();

		private:
			bool (*m_Operation)(Instruction &); // Function pointer to execute the instruction

			uint8_t rs1, rs2, rd;
			uint16_t imm;
		};
		class Parser
		{
		public:
			Parser(std::istream &inputStream);
			Instruction ReadNextLine(); // last instruction will always be a NOP
			Instruction ParseInstruction(const std::string &instructionLine);
			std::stringstream SanitizeStream();

		private:
			std::istream &m_InputStream;
			std::unordered_map<std::string, uint32_t> m_LabelsMap;
		};

		struct File
		{
			std::vector<Instruction> m_Instructions;
		};

	}
}