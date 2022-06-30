#include "Parser/RegisterFile.h"
#define STR(reg) #reg

const std::vector<std::string> RegisterFile::regNamesTrue = {
	STR(x0),
	STR(x1),
	STR(x2),
	STR(x3),
	STR(x4),
	STR(x5),
	STR(x6),
	STR(x7),
	STR(x8),
	STR(x9),
	STR(x10),
	STR(x11),
	STR(x12),
	STR(x13),
	STR(x14),
	STR(x15),
	STR(x16),
	STR(x17),
	STR(x18),
	STR(x19),
	STR(x20),
	STR(x21),
	STR(x22),
	STR(x23),
	STR(x24),
	STR(x25),
	STR(x26),
	STR(x27),
	STR(x28),
	STR(x29),
	STR(x30),
	STR(x31),
};
const std::vector<std::string> RegisterFile::regNamesAPI = {
	STR(zero),
	STR(ra),
	STR(sp),
	STR(gp),
	STR(tp),
	STR(t0),
	STR(t1),
	STR(t2),
	STR(s0),
	STR(s1),
	STR(a0),
	STR(a1),
	STR(a2),
	STR(a3),
	STR(a4),
	STR(a5),
	STR(a6),
	STR(a7),
	STR(s2),
	STR(s3),
	STR(s4),
	STR(s5),
	STR(s6),
	STR(s7),
	STR(s7),
	STR(s8),
	STR(s9),
	STR(s10),
	STR(s11),
	STR(t3),
	STR(t4),
	STR(t5),
	STR(t6),
};

RegisterFile &RegisterFile::Instance()
{
	static RegisterFile instance = RegisterFile();
	return instance;
}

RegisterFile::RegisterFile()
{
	for (uint32_t i = 0; i < 32; i++)
	{
		m_Regs[i] = 0;
		trueToApiMap.insert(regNamesTrue[i], regNamesAPI[i]);
		APIToTrueMap.insert(regNamesAPI[i], regNamesTrue[i]);
	}
}

std::string RegisterFile::GetAPIRegName(const std::string &reg)
{
	auto &map = Instance().trueToApiMap;
	if (auto &it = map.find(reg); it != map.end())
		return it->second;
	else
		return reg;
}

std::string RegisterFile::GetTrueRegName(const std::string &reg)
{
	auto &map = Instance().APIToTrueMap;
	if (auto &it = map.find(reg); it != map.end())
		return it->second;
	else
		return reg;
}

uint32_t RegisterFile::GetValue(uint32_t index)
{
	if (index >= 32)
		throw "INDEX OUT OF RANGE";

	return Instance().m_Regs[index];
}

void RegisterFile::SetValue(uint32_t index, uint32_t value)
{
	if (index >= 32)
		throw "INDEX OUT OF RANGE";

	Instance().m_Regs[index] = value;
}