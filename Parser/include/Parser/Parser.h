#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <string_view>

struct Token;

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
		Token *token;
		inline ErrorMessage(Type t = NO_ERROR) : type(t), token(nullptr) {}
		operator bool();
	};

	std::ostream &operator<<(std::ostream &stream, const ErrorMessage &message);

	class Parser
	{
	public:
		Parser();
		bool ReadFile(std::string_view filePath);
		void CleanParserData();
		void PrintFileContent() const;
		ErrorMessage Parse();
		~Parser();

	private:
		struct CharacterIndex
		{
			char c;
			uint32_t index;
		};
		CharacterIndex FindFirstOfTokens(const std::string &str);
		void SanitizeFileContent();

	private:
		std::string m_FileContent;
		std::vector<Token *> m_Tokens;
	};
}