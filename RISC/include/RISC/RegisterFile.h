#pragma once
#include <array>
#include <string>
#include <vector>
#include <map>

namespace RISC
{
	class RegisterFile
	{
	public:
		static const std::vector<std::string> regNamesTrue;
		static const std::vector<std::string> regNamesAPI;

		static RegisterFile &Instance();
		static std::string GetAPIRegName(const std::string &reg);
		static std::string GetTrueRegName(const std::string &reg);
		static int IsRegister(const std::string &str);
		static uint32_t GetValue(uint32_t index);
		static void SetValue(uint32_t index, uint32_t value);
		static uint32_t GetRegisterIndex(const std::string &reg);

	private:
		RegisterFile();

		std::map<std::string, std::string> trueToApiMap;
		std::map<std::string, std::string> APIToTrueMap;
		std::map<std::string, uint32_t> trueToIndexMap;
		std::array<uint32_t, 32> m_Regs;
	};
}