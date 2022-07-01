#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <string_view>
#include "RISC/Instruction.h"

namespace Parser
{
	struct ErrorMessage
	{
		enum Type : uint32_t
		{
			NO_ERROR,
			INVALID_LABEL,
			INVALID_SYMBOL,
			INVALID_INSTRUCTION,
			INVALID_PARAMETERS,
			COUNT
		} type = NO_ERROR;
		inline ErrorMessage(Type t = NO_ERROR) : type(t) {}
		operator bool();
	};

	std::ostream &operator<<(std::ostream &stream, const ErrorMessage &message);

	struct ParseOutput
	{
		inline ParseOutput(ErrorMessage err = ErrorMessage(ErrorMessage::NO_ERROR)) : err(err) {}
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

	private:
		std::string m_FileContent;
		std::string m_ParsableFile;
	};
}