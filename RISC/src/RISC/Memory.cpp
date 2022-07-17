#include <RISC/Memory.h>

#include <cstring>

namespace RISC
{
	const uint32_t Memory::GetDatStartAddr()
	{
		return Get().m_DataStartAddr;
	}

	const uint32_t Memory::GetTextStartAddr()
	{
		return Get().m_TextStartAddr;
	}

	const uint32_t Memory::GetHeapStartAddr()
	{
		return Get().m_HeapStartAddr;
	}

	const uint32_t Memory::GetWord(uint32_t address, bool isData)
	{
		auto &memBlocks = Get().m_Blocks;
		if (Get().CheckValidity(address, isData))
		{
			return (memBlocks[address] << 24) | (memBlocks[address + 1] << 16) |
				   (memBlocks[address + 2] << 8) | (memBlocks[address + 3]);
		}
		else
			return UINT_MAX;
	}

	const uint16_t Memory::GetHalfWord(uint32_t address, bool isData)
	{
		auto &memBlocks = Get().m_Blocks;
		if (Get().CheckValidity(address, isData))
		{
			return (memBlocks[address] << 8) | (memBlocks[address + 1]);
		}
		else
			return UINT16_MAX;
	}

	const uint8_t Memory::GetByte(uint32_t address, bool isData)
	{
		auto &memBlocks = Get().m_Blocks;
		if (Get().CheckValidity(address, isData))
		{
			return memBlocks[address];
		}
		else
			return UINT8_MAX;
	}

	const uint32_t Memory::GetHalfWordE(uint32_t address, bool isData)
	{
		auto &memBlocks = Get().m_Blocks;
		if (Get().CheckValidity(address, isData))
		{
			uint32_t value = (memBlocks[address] << 8) | (memBlocks[address + 1]);
			if (value & 0x8000)
				value |= 0xffff0000; // sign extend

			return value;
		}
		else
			return UINT_MAX;
	}

	const uint32_t Memory::GetByteE(uint32_t address, bool isData)
	{
		auto &memBlocks = Get().m_Blocks;
		if (Get().CheckValidity(address, isData))
		{
			uint32_t value = (memBlocks[address]);
			if (value & 0x80)
				value |= 0xffffff00; // sign extend

			return value;
		}
		else
			return UINT_MAX;
	}

	const bool Memory::SaveByte(uint32_t address, uint8_t byte, bool isData)
	{
		auto &memBlocks = Get().m_Blocks;

		if (Get().CheckValidity(address, isData))
		{
			memBlocks[address] = byte;
			return true;
		}
		else
			return false;
	}

	const bool Memory::SaveHalfWord(uint32_t address, uint16_t halfWord, bool isData)
	{
		auto &memBlocks = Get().m_Blocks;

		if (Get().CheckValidity(address, isData))
		{
			memBlocks[address] = halfWord & 0xff;
			memBlocks[address + 1] = (halfWord & 0xff00) >> 8;
			return true;
		}
		else
			return false;
	}

	const bool Memory::SaveWord(uint32_t address, uint32_t word, bool isData)
	{
		auto &memBlocks = Get().m_Blocks;

		if (Get().CheckValidity(address, isData))
		{
			memBlocks[address] = uint8_t(word & 0xff);
			memBlocks[address + 1] = uint8_t((word & 0xff00) >> 8);
			memBlocks[address + 2] = uint8_t((word & 0xff0000) >> 16);
			memBlocks[address + 3] = uint8_t((word & 0xff000000) >> 24);
			return true;
		}
		else
			return false;
	}

	Memory &Memory::Get()
	{
		static Memory mem{};
		return mem;
	}

	Memory::Memory()
	{
		// Initializing the entire meomry with zeros
		m_Blocks.resize(MEMORY_SIZE, 0);
	}

	bool Memory::CheckValidity(uint32_t address, bool isData)
	{
		// to be visited later with more sophisticated memory preserving techniques (you can't touch memory that is not yours)
		if (isData)
			return ((address >= m_DataStartAddr && address < m_HeapStartAddr) || (address >= m_HeapStartAddr && address < m_GpStartAddr));
		else
			return ((address >= m_TextStartAddr && address < m_DataStartAddr));
	}
}