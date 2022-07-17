#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include <array>

// 4 GB
#define MEMORY_SIZE (size_t)0x100000000

namespace RISC
{
	// Handle the errors in a better way (wrap them in a struct that takes in the output + an error message.)
	class Memory
	{
	public:
		static const uint32_t GetDatStartAddr();
		static const uint32_t GetTextStartAddr();
		static const uint32_t GetHeapStartAddr();
		static const uint32_t GetWord(uint32_t address, bool isData);
		static const uint16_t GetHalfWord(uint32_t address, bool isData);
		static const uint8_t GetByte(uint32_t address, bool isData);
		static const uint32_t GetHalfWordE(uint32_t address, bool isData);
		static const uint32_t GetByteE(uint32_t address, bool isData);
		static const bool SaveByte(uint32_t address, uint8_t byte, bool isData);
		static const bool SaveHalfWord(uint32_t address, uint16_t halfWord, bool isData);
		static const bool SaveWord(uint32_t address, uint32_t word, bool isData);

	private:
		static Memory &Get();
		Memory();

		bool CheckValidity(uint32_t address, bool isData);

		const uint32_t m_TextStartAddr = 0x00400000;
		const uint32_t m_DataStartAddr = 0x10010000;
		const uint32_t m_HeapStartAddr = 0x10040000;
		const uint32_t m_GpStartAddr = 0x10008000;
		std::vector<uint8_t> m_Blocks;
	};
}