#pragma once
#include <stdarg.h>
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
			INVALID_STRUCTURE_DATA,
			INVALID_STRUCTURE_TEXT,
			COUNT
		} type = NO_ERROR;
		char *msg;
		inline ErrorMessage(Type t = NO_ERROR, const char *fmt = nullptr, ...) : type(t)
		{
			msg = new char[1024];
			memset(msg, 0, 1024);
			if (fmt != nullptr)
			{
				va_list args;
				va_start(args, fmt);
				vsprintf(msg, fmt, args);
				va_end(args);
			}
		}
		inline ErrorMessage(ErrorMessage &&e)
		{
			msg = e.msg;
			e.msg = nullptr;
			type = e.type;
		}
		inline operator bool()
		{
			return type != NO_ERROR;
		}
		inline ErrorMessage &operator=(const ErrorMessage &e)
		{
			if (this->msg == e.msg && this->type == e.type)
				return *this;
			msg = new char[1024];
			memset(msg, 0, 1024);
			for (uint32_t i = 0; i < 1024; i++)
			{
				msg[i] = e.msg[i];
			}
			type = e.type;
			return *this;
		}
		inline ErrorMessage &operator=(ErrorMessage &&e)
		{
			msg = e.msg;
			e.msg = nullptr;
			type = e.type;
			return *this;
		}

		inline ~ErrorMessage()
		{
			delete[] msg;
		}
	};

	std::ostream &operator<<(std::ostream &stream, const ErrorMessage &message);

	struct ParseOutput
	{
		inline ParseOutput(ErrorMessage err = ErrorMessage(ErrorMessage::NO_ERROR)) { this->err = err; }
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