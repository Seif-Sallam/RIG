#include "RISC/DataSectionContainer.h"

#include <Utils/Logger.h>

typedef Util::Logger Log;
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
		Log::Info("Items:");
		for (auto &item : m_Items)
		{
			fmt::print("\tLabel: {}\n", item.m_Name);
			fmt::print("\tSize: {}\n", item.m_Size);
			switch (item.m_Type)
			{
			case DataItem::WORD:
			{
				fmt::print("\tType: Word\n");
				fmt::print("\tData:\v");
				fmt::print("\t{{ ");
				uint32_t *data = (uint32_t *)item.m_Data;
				for (uint32_t i = 0; i < item.m_Size; i++)
				{
					fmt::print("{}", data[i]);
					if (i != item.m_Size - 1)
						fmt::print(",");
				}
				fmt::print(" }}\n");
			}
			break;
			case DataItem::HALF_WORD:
			{
				fmt::print("\tType: Half Word\n");
				fmt::print("\tData:\v");
				fmt::print("\t{{ ");
				uint16_t *data = (uint16_t *)item.m_Data;
				for (uint32_t i = 0; i < item.m_Size; i++)
				{
					fmt::print("{}", data[i]);
					if (i != item.m_Size - 1)
						fmt::print(", ");
				}
				fmt::print(" }}\n");
			}
			break;
			case DataItem::BYTE:
			{
				fmt::print("\tType: Byte\n");
				fmt::print("\tData:\v");
				fmt::print("\t{{ ");
				uint8_t *data = (uint8_t *)item.m_Data;
				for (uint32_t i = 0; i < item.m_Size; i++)
				{
					fmt::print("{}", data[i]);
					if (i != item.m_Size - 1)
						fmt::print(", ");
				}
				fmt::print(" }}\n");
			}
			break;
			case DataItem::SPACE:
			{
				fmt::print("\tType: Space\n");
				fmt::print("\tData:\v");
				fmt::print("\t{{ ");
				uint32_t data = ((uint32_t *)item.m_Data)[0];
				fmt::print("{} }}\n", data);
			}
			break;
			case DataItem::FLOAT:
			{
				fmt::print("\tType: Float\n");
				fmt::print("\tData:\v");
				fmt::print("\t{{ ");
				float *data = (float *)item.m_Data;
				for (uint32_t i = 0; i < item.m_Size; i++)
				{
					fmt::print("{}", data[i]);
					if (i != item.m_Size - 1)
						fmt::print(", ");
				}
				fmt::print(" }}\n");
			}
			break;
			case DataItem::DOUBLE:
			{
				fmt::print("\tType: Double\n");
				fmt::print("\tData:\v");
				fmt::print("\t{{ ");
				double *data = (double *)item.m_Data;
				for (uint32_t i = 0; i < item.m_Size; i++)
				{
					fmt::print("{}", data[i]);
					if (i != item.m_Size - 1)
						fmt::print(", ");
				}
				fmt::print(" }}\n");
			}
			break;
			case DataItem::STRING:
			{
				fmt::print("\tType: String\n");
				fmt::print("\tData:\v");
				fmt::print("\t{{ \"");
				char *data = (char *)item.m_Data;
				for (uint32_t i = 0; i < item.m_Size; i++)
				{
					fmt::print("{}", data[i]);
				}
				fmt::print("\" }}\n");
			}
			break;
			case DataItem::C_STRING:
			{
				fmt::print("\tType: C-String\n");
				fmt::print("\tData:\v");
				fmt::print("\t{{ \"");
				char *data = (char *)item.m_Data;
				fmt::print(data);
				fmt::print("\" }}\n");
			}
			break;
			}
		}
	}
}