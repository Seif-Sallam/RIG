#pragma once

#include <string>
#include <vector>
#include <string_view>

#include <Parser/ErrorMessage.h>

#include "RISC/Instruction.h"

namespace Parser
{

	struct Label
	{
		std::string labelString;
		uint32_t address;
	};

	struct DataLabel : Label
	{
		enum DataType
		{
			WORD,
			HALF_WORD,
			BYTE,
			STRING,
			C_STRING,
			DOUBLE,
			FLOAT,
			SPACE,
		} type;
		std::string data;
	};

	struct ParseOutput
	{
		inline ParseOutput(ErrorMessage errMsg = ErrorMessage(ErrorMessage::NO_ERROR)) : err(errMsg) {}
		uint32_t dataSectionSize = 0;
		std::vector<Label> instLabels;
		std::vector<DataLabel> dataLabels;
		std::vector<Instruction> instructions;
		ErrorMessage err;
	};

	class Parser
	{
	public:
		Parser();
		bool ReadFile(std::string_view filePath);
		void CleanParserData();
		void PrintFileContent() const;
		ParseOutput Parse();
		~Parser();

		void FormatFile();

	private:
		void SanitizeFileContent(std::string &file, bool removeComments);
		ErrorMessage SanitizeDataSection(std::string &dataSection);

		struct FileText
		{
			std::string textSection;
			std::string dataSection;
			ErrorMessage err;
		};

		FileText GenerateTextAndData(const std::string &fileContent);

	private:
		std::string m_FileContent;
		std::string m_ParsableFile;
	};
}