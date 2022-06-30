#include "Parser/Parser.h"
#include "Parser/Token.h"
#include <map>

#define MAP_ENTRY(enumName) \
	{                       \
		enumName, #enumName \
	}
namespace Parser
{

	Parser::Parser()
		: m_FileContent(""), m_Tokens(256, {})
	{
	}

	bool Parser::ReadFile(std::string_view filePath)
	{
		std::ifstream inputFile(filePath.data(), std::ios::binary);
		if (inputFile.is_open())
		{
			uint32_t size = inputFile.tellg();
			m_FileContent = std::string(std::istreambuf_iterator<char>(inputFile), std::istreambuf_iterator<char>());
			inputFile.close();
			m_FileContent.erase(std::remove(m_FileContent.begin(), m_FileContent.end(), '\r'), m_FileContent.end());
			SanitizeFileContent();
			return true;
		}
		else
		{
			return false;
		}
	}

	void Parser::CleanParserData()
	{
		m_FileContent.clear();
		for (auto &token : m_Tokens)
		{
		}
	}

	void Parser::PrintFileContent() const
	{
		std::cout << m_FileContent << std::endl;
	}

	Parser::~Parser()
	{
		CleanParserData();
	}

	void Parser::SanitizeFileContent()
	{
		bool inComment = false;
		bool foundWhiteSpace = false;
		bool fonudNewLine = false;
		for (int i = 0; i < m_FileContent.size(); i++)
		{
			char &c = m_FileContent[i];
			switch (c)
			{
			case '\t':
				c = ' ';
				i--;
				continue;
				break;
			case '#':
				inComment = true;
				break;
			case ':':
				if (inComment)
					break;
				m_FileContent.insert(m_FileContent.begin() + i + 1, '\n');
				foundWhiteSpace = false;
				fonudNewLine = false;
				break;
			case '\n':
				foundWhiteSpace = false;
				inComment = false;
				if (fonudNewLine)
					c = char(27);
				else
					fonudNewLine = true;
				break;
			case ' ':
				if (foundWhiteSpace || fonudNewLine)
				{
					c = char(27);
					fonudNewLine = false;
				}
				else
					foundWhiteSpace = true;
				break;
			default:
				foundWhiteSpace = false;
				fonudNewLine = false;
			}
		}

		auto it = std::remove_if(m_FileContent.begin(), m_FileContent.end(), [](const char &c)
								 { return c == (char)27; });

		m_FileContent.erase(it, m_FileContent.end());
	}

	ErrorMessage Parser::Parse()
	{
		std::stringstream ss;
		ss << m_FileContent;
		while (!ss.eof())
		{
			std::string line;
			std::getline(ss, line, '\n');
			auto delim = FindFirstOfTokens(line);
			switch (delim.c)
			{
			case ':':
				break;
			case '#':
				break;
			case '(':
				break;
			case ')':
				break;
			case '.':
				break;
			case ' ':
				break;
			}
		}
		return {};
	}

	const std::string g_Delimnators = ":#(). ";

	Parser::CharacterIndex Parser::FindFirstOfTokens(const std::string &str)
	{
		CharacterIndex ci = {UCHAR_MAX, UINT_MAX};
		for (int i = 0; i < str.size(); i++)
		{
			if (g_Delimnators.find(str[i]) != std::string::npos)
			{
				ci.c = str[i];
				ci.index = i;
				return ci;
			}
		}
		return ci;
	}

	const std::map<ErrorMessage::Type, const char *> enumNames{
		MAP_ENTRY(ErrorMessage::INVALID_INSTRUCTION),
		MAP_ENTRY(ErrorMessage::INVALID_LABEL),
		MAP_ENTRY(ErrorMessage::INVALID_PARAMETERS),
		MAP_ENTRY(ErrorMessage::INVALID_SYMBOL),
		MAP_ENTRY(ErrorMessage::NO_ERROR),
	};

	std::ostream &operator<<(std::ostream &stream, const ErrorMessage &message)
	{
		stream << "Error: " << enumNames.at(message.type) << "\n\tToken: " << message.token->data << '\n';
		return stream;
	}

	ErrorMessage::operator bool()
	{
		return type == NO_ERROR;
	}
}