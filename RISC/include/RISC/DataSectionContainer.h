#pragma once
#include <stdint.h>
#include <vector>
#include <iostream>
#include <string>

namespace RISC
{
	class DataSectionContainer;

	struct DataItem
	{
	public:
		enum Type : uint8_t
		{
			WORD,
			HALF_WORD,
			BYTE,
			STRING,
			C_STRING,
			DOUBLE,
			FLOAT,
			SPACE,
		};
		const Type GetType() const;
		DataItem(const std::string &name, Type type, uint32_t size);

		void *Data() const
		{
			return m_Data;
		}

		DataItem(DataItem &&d2);
		void Fill(void *&data);
		~DataItem();

	private:
		Type m_Type;
		uint32_t m_Size;
		void *m_Data;
		std::string m_Name;
		friend DataSectionContainer;
	};

	class DataSectionContainer
	{
	public:
		// Moves the array given in the data parameter ENTIRELY to the DataItem container.
		// The array outside would not be valid after that.
		void AddItem(std::string label, DataItem::Type type, uint32_t size, void *&data);
		void Clear();
		void Print();

	private:
		std::vector<DataItem> m_Items;
	};

}