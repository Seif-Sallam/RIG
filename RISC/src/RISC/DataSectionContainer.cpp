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
			Log::Print("\tLabel: {}", item.m_Name);
			Log::Print("\tSize: {}", item.m_Size);
			switch (item.m_Type)
			{
			case DataItem::WORD:
			{
				Log::Print("\tType: Word");
				Log::Print("\tData:");
				uint32_t *data = (uint32_t *)item.m_Data;
				std::string dataStr;
				for (uint32_t i = 0; i < item.m_Size; i++)
				{
					dataStr += std::to_string(data[i]) + ",";
					if (i == item.m_Size - 1)
						dataStr.pop_back();
				}
				Log::Print("\t\t{{ {} }}", dataStr);
			}
			break;
			case DataItem::HALF_WORD:
			{
				Log::Print("\tType: Half Word");
				Log::Print("\tData:");
				uint16_t *data = (uint16_t *)item.m_Data;
				std::string dataStr;
				for (uint32_t i = 0; i < item.m_Size; i++)
				{
					dataStr += std::to_string(data[i]) + ",";
					if (i == item.m_Size - 1)
						dataStr.pop_back();
				}
				Log::Print("\t\t{{ {} }}", dataStr);
			}
			break;
			case DataItem::BYTE:
			{
				Log::Print("\tType: Byte");
				Log::Print("\tData:");
				uint8_t *data = (uint8_t *)item.m_Data;
				std::string dataStr;
				for (uint32_t i = 0; i < item.m_Size; i++)
				{
					dataStr += std::to_string(data[i]) + ",";
					if (i == item.m_Size - 1)
						dataStr.pop_back();
				}
				Log::Print("\t\t{{ {} }}", dataStr);
			}
			break;
			case DataItem::SPACE:
			{
				Log::Print("\tType: Space");
				Log::Print("\tData:");
				uint32_t data = ((uint32_t *)item.m_Data)[0];
				Log::Print("\t\t{{ {} }}", data);
			}
			break;
			case DataItem::FLOAT:
			{
				Log::Print("\tType: Float");
				Log::Print("\tData:");
				float *data = (float *)item.m_Data;
				std::string dataStr;
				for (uint32_t i = 0; i < item.m_Size; i++)
				{
					dataStr += std::to_string(data[i]) + ",";
					if (i == item.m_Size - 1)
						dataStr.pop_back();
				}
				Log::Print("\t\t{{ {} }}", dataStr);
			}
			break;
			case DataItem::DOUBLE:
			{
				Log::Print("\tType: Double");
				Log::Print("\tData:");
				double *data = (double *)item.m_Data;
				std::string dataStr;
				for (uint32_t i = 0; i < item.m_Size; i++)
				{
					dataStr += std::to_string(data[i]) + ",";
					if (i == item.m_Size - 1)
						dataStr.pop_back();
				}
				Log::Print("\t\t{{ {} }}", dataStr);
			}
			break;
			case DataItem::STRING:
			{
				Log::Print("\tType: String");
				Log::Print("\tData:");
				char *data = new char[item.m_Size + 1];
				memcpy((void *)data, (void *)item.m_Data, item.m_Size);
				data[item.m_Size] = '\0';
				Log::Print("\t\t{{ \"{}\" }}", data);
			}
			break;
			case DataItem::C_STRING:
			{
				Log::Print("\tType: C-String");
				Log::Print("\tData:");
				char *data = (char *)item.m_Data;
				Log::Print("\t\t{{ \"{}\" }}", data);
			}
			break;
			}
		}
	}
}