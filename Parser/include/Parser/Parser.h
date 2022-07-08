#pragma once

#include <string>
#include <vector>
#include <string_view>

#include <Parser/ErrorMessage.h>

#include "RISC/Instruction.h"

namespace Parser
{
	struct ParseOutput
	{
		inline ParseOutput(ErrorMessage errMsg = ErrorMessage(ErrorMessage::NO_ERROR)) : err(errMsg) {}
		std::vector<std::string> labels;
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