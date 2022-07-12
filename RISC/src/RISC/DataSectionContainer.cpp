#include "RISC/DataSectionContainer.h"

namespace RISC
{
	const DataItem::Type DataItem::GetType() const { return m_Type; }

	DataItem::DataItem(const std::string &name, DataItem::Type type, uint32_t size)
		: m_Size(size), m_Type(type), m_Data(nullptr), m_Name(name)
	{
	}

	DataItem::DataItem(DataItem &&d2)
	{
		this->m_Size = d2.m_Size;
		this->m_Type = d2.m_Type;
		this->m_Data = d2.m_Data; // this works because this is a union
		this->m_Name = std::move(d2.m_Name);
		d2.m_Data = nullptr;
	}

	void DataItem::Fill(void *&data)
	{
		if (m_Data != nullptr)
		{
			delete[](char *) m_Data;
			m_Data = nullptr;
		}
		m_Data = data;
		data = nullptr;
	}

	DataItem::~DataItem()
	{
		free(m_Data);
	}

	void DataSectionContainer::AddItem(std::string label, DataItem::Type type, uint32_t size, void *&data)
	{
		m_Items.emplace_back(label, type, size);
		m_Items.back().Fill(data);
	}

	void DataSectionContainer::Clear()
	{
		m_Items.clear();
	}

	void DataSectionContainer::Print()
	{
		std::cout << "Items:\n";
		for (auto &item : m_Items)
		{
			std::cout << "\tLabel: " << item.m_Name << '\n';
			std::cout << "\tSize: " << item.m_Size << '\n';
			switch (item.m_Type)
			{
			case DataItem::WORD:
			{
				std::cout << "\tType: Word\n";
				std::cout << "\tData:\v";
				std::cout << "\t{ ";
				uint32_t *data = (uint32_t *)item.m_Data;
				for (uint32_t i = 0; i < item.m_Size; i++)
				{
					std::cout << data[i];
					if (i != item.m_Size - 1)
						std::cout << ", ";
				}
				std::cout << " }\n";
			}
			break;
			case DataItem::HALF_WORD:
			{
				std::cout << "\tType: Half Word\n";
				std::cout << "\tData:\v";
				std::cout << "\t{ ";
				uint16_t *data = (uint16_t *)item.m_Data;
				for (uint32_t i = 0; i < item.m_Size; i++)
				{
					std::cout << data[i];
					if (i != item.m_Size - 1)
						std::cout << ", ";
				}
				std::cout << " }\n";
			}
			break;
			case DataItem::BYTE:
			{
				std::cout << "\tType: Byte\n";
				std::cout << "\tData:\v";
				std::cout << "\t{ ";
				uint8_t *data = (uint8_t *)item.m_Data;
				for (uint32_t i = 0; i < item.m_Size; i++)
				{
					std::cout << data[i];
					if (i != item.m_Size - 1)
						std::cout << ", ";
				}
				std::cout << " }\n";
			}
			break;
			case DataItem::SPACE:
			{
				std::cout << "\tType: Space\n";
				std::cout << "\tData:\v";
				std::cout << "\t{ ";
				uint32_t data = ((uint32_t *)item.m_Data)[0];
				std::cout << data << " }\n";
			}
			break;
			case DataItem::FLOAT:
			{
				std::cout << "\tType: Float\n";
				std::cout << "\tData:\v";
				std::cout << "\t{ ";
				float *data = (float *)item.m_Data;
				for (uint32_t i = 0; i < item.m_Size; i++)
				{
					std::cout << data[i];
					if (i != item.m_Size - 1)
						std::cout << ", ";
				}
				std::cout << " }\n";
			}
			break;
			case DataItem::DOUBLE:
			{
				std::cout << "\tType: Double\n";
				std::cout << "\tData:\v";
				std::cout << "\t{ ";
				double *data = (double *)item.m_Data;
				for (uint32_t i = 0; i < item.m_Size; i++)
				{
					std::cout << data[i];
					if (i != item.m_Size - 1)
						std::cout << ", ";
				}
				std::cout << " }\n";
			}
			break;
			case DataItem::STRING:
			{
				std::cout << "\tType: String\n";
				std::cout << "\tData:\v";
				std::cout << "\t{ \"";
				char *data = (char *)item.m_Data;
				for (uint32_t i = 0; i < item.m_Size; i++)
				{
					std::cout << data[i];
				}
				std::cout << "\" }\n";
			}
			break;
			case DataItem::C_STRING:
			{
				std::cout << "\tType: C-String\n";
				std::cout << "\tData:\v";
				std::cout << "\t{ \"";
				char *data = (char *)item.m_Data;
				std::cout << data;
				std::cout << "\" }\n";
			}
			break;
			}
		}
	}
}