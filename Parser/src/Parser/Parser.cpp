#include <fstream>
#include <sstream>
#include <math.h>
#include <cstring>
#include <algorithm>
#include <regex>

#include <Parser/Parser.h>
#include <Parser/ErrorMessage.h>

#include <RISC/RegisterFile.h>
#include <RISC/Instruction.h>
#include <RISC/DataSectionContainer.h>

#include <Utils/Logger.h>

typedef Util::Logger Log;

namespace Parser
{

	inline static bool IsInteger(const char *value)
	{
		bool hex = false;
		bool isBinary = false;
		bool isNegative = value[0] == '-';

		if (value[0 + isNegative] == '0' && tolower(value[1 + isNegative]) == 'x')
			hex = true;
		else if (value[0 + isNegative] == '0' && tolower(value[1 + isNegative]) == 'b')
			isBinary = true;

		uint32_t start = (hex || isBinary) ? 2 : 0;
		start += isNegative;
		for (auto p = value + start; *p != '\0'; p++)
		{
			char c = *p;
			c = tolower(c);
			if (isBinary)
			{
				if (!(c == '0' || c == '1'))
					return false;
			}
			else if (hex == true)
			{
				if (!isdigit(c) && (c > 'f' || c < 'a'))
					return false;
			}
			else
			{
				if (!isdigit(c))
					return false;
			}
		}
		return true;
	}

	inline static uint32_t GetInteger(const char *imm, uint32_t size)
	{
		uint32_t sum = 0;
		uint32_t base = 10;
		uint32_t pos = 0;
		bool isHex = false;
		bool isBinary = false;
		bool isNegative = imm[0] == '-';
		if (imm[0 + isNegative] == '0' && tolower(imm[1 + isNegative]) == 'x')
		{
			base = 16;
			isHex = true;
		}
		if (imm[0 + isNegative] == '0' && tolower(imm[1 + isNegative]) == 'b')
		{
			base = 2;
			isBinary = true;
		}
		int32_t start = (isHex || isBinary) ? 2 : 0;
		start += isNegative;
		for (int32_t i = size - 1; i >= start; i--)
		{
			char c = imm[i];
			if (c == '\0')
				continue;
			c = tolower(c);

			uint32_t value = c - '0';
			if (isHex)
			{
				if (isalpha(c))
					value = c - 'a' + 10;
			}

			sum += value * (uint32_t)pow(base, pos);
			pos++;
		}

		if (isNegative)
			sum *= -1;
		return sum;
	}

	Parser::Parser()
		: m_FileContent("")
	{
	}

	bool Parser::ReadFile(std::string_view filePath)
	{
		std::ifstream inputFile(filePath.data(), std::ios::binary);
		if (inputFile.is_open())
		{
			size_t size = inputFile.tellg();
			m_FileContent = std::string(std::istreambuf_iterator<char>(inputFile), std::istreambuf_iterator<char>());
			inputFile.close();

			m_FileContent.erase(std::remove(m_FileContent.begin(), m_FileContent.end(), '\r'), m_FileContent.end());
			m_ParsableFile = m_FileContent;
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
	}

	void Parser::PrintFileContent() const
	{
		Log::Print("{}", m_FileContent);
	}

	Parser::~Parser()
	{
		CleanParserData();
	}

	inline static void AddNewLineAfter(std::string &str, const char *pattern)
	{
		size_t findIndex = str.find(pattern);
		if (findIndex != std::string::npos)
		{
			if (findIndex - str.size() < 10)
			{
				size_t index = findIndex + strlen(pattern);
				if (str[index] != '\n')
					str.insert(str.begin() + index, '\n');
			}
		}
	}

	void Parser::SanitizeFileContent(std::string &fileContent, bool removeComments)
	{
		bool inComment = false;
		bool foundWhiteSpace = false;
		bool fonudNewLine = false;
		bool openedParn = false;
		uint32_t whiteSpaceStart = 0;
		int32_t startComment = 0;
		uint32_t newLineCount = 0;
		char lastCharacter = 0;
		bool rmEntireLine = false;

		for (uint32_t i = 0; i < fileContent.size(); i++)
		{
			char &c = fileContent[i];
			char ogC = c;
			switch (c)
			{
			case ')':
				foundWhiteSpace = false;
				openedParn = false;
				break;
			case '(':
				if (foundWhiteSpace)
				{
					fileContent[whiteSpaceStart] = char(27);
				}
				foundWhiteSpace = false;
				openedParn = true;
				break;
			case ',':
				if (inComment)
					break;
				foundWhiteSpace = false;
				fileContent.insert(fileContent.begin() + i + 1, ' ');
				break;
			case '\t':
				c = ' ';
				i--;
				continue;
				break;
			case '#':
				if (!inComment)
				{
					startComment = i;
					if (lastCharacter == '\n' || lastCharacter == 0)
					{

						rmEntireLine = true;
					}
					else if (!foundWhiteSpace)
					{
						fileContent.insert(fileContent.begin() + i, ' ');
						i++;
					}

					if (foundWhiteSpace)
						startComment = whiteSpaceStart;
				}
				inComment = true;

				break;
			case ':':
				if (inComment)
					break;
				if (foundWhiteSpace)
					fileContent[whiteSpaceStart] = char(27);
				fileContent.insert(fileContent.begin() + i + 1, '\n');
				foundWhiteSpace = false;
				fonudNewLine = false;
				newLineCount = 0;
				break;
			case '\n':
			{
				newLineCount++;
				if (lastCharacter == 0)
				{
					c = char(27);
				}
				if (foundWhiteSpace)
				{
					for (uint32_t index = whiteSpaceStart; index < i; index++)
					{
						fileContent[index] = char(27);
					}
				}
				openedParn = false;
				foundWhiteSpace = false;
				if (inComment)
				{
					if (removeComments)
					{
						for (uint32_t index = startComment; index < i; index++)
							fileContent[index] = char(27);
						if (rmEntireLine)
							c = char(27);
					}
					rmEntireLine = false;
				}

				inComment = false;
				if (fonudNewLine && newLineCount >= (2u + !removeComments))
					c = char(27);
				else
					fonudNewLine = true;
			}
			break;
			case ' ':
				if (inComment)
					break;
				if (openedParn)
				{
					c = char(27);
				}
				if (foundWhiteSpace || fonudNewLine)
				{
					c = char(27);
					fonudNewLine = false;
					newLineCount = 0;
					if (fonudNewLine)
						whiteSpaceStart = i;
				}
				else
				{
					whiteSpaceStart = i;
				}
				foundWhiteSpace = true;
				break;
			default:
				foundWhiteSpace = false;
				fonudNewLine = false;
				newLineCount = 0;
			}
			lastCharacter = ogC;
		}

		auto it = std::remove_if(fileContent.begin(), fileContent.end(), [](const char &c)
								 { return c == (char)27; });
		fileContent.erase(it, fileContent.end());
		// Adding a new line after the .data and the .text if there is not
		{
			AddNewLineAfter(fileContent, ".data");
			AddNewLineAfter(fileContent, ".text");
			/*
				List of some of the directives
			*/
			AddNewLineAfter(fileContent, ".macro");
			AddNewLineAfter(fileContent, ".end_macro");
		}
	}

	ErrorMessage Parser::SanitizeDataSection(std::string &dataSection)
	{
		std::string finalDataSection = "";
		std::stringstream ss;
		ss << dataSection;
		while (!ss.eof())
		{
			std::string label;
			std::getline(ss, label);
			if (label.empty())
				continue;
			label.pop_back();

			if (!std::regex_match(label, std::regex("([a-zA-Z_][a-zA-Z0-9_]*)")))
				return ErrorMessage{ErrorMessage::INVALID_LABEL, "Invalid label naming: %s", label.c_str()};

			finalDataSection += label + ":\n";

			std::string directive;
			std::getline(ss, directive);
			std::string data;
			{
				size_t spaceIndex = directive.find(" ");
				data = directive.substr(spaceIndex + 1);
				directive = directive.substr(0, spaceIndex);
			}
			if (directive == ".asciz" || directive == ".ascii" || directive == ".string")
			{
				uint32_t numOfQuotes = 0;
				char quoteType = 0;
				for (uint32_t i = 0; i < data.size(); i++)
				{
					if (data[i] == '\"' || data[i] == '\'')
					{
						if (quoteType == 0)
							quoteType = data[i];

						if (numOfQuotes >= 1)
						{
							if ((i != data.size() - 1) && data[i - 1] != '\\')
								return ErrorMessage{ErrorMessage::INVALID_DIRECTIVE_DATA, "Invalid directive data (Escape Character not found): %s\n\t at index: %u", data.c_str(), i};
							if (i == data.size() - 1 && (data[i - 1] == '\\' || quoteType != data[i]))
								return ErrorMessage{ErrorMessage::INVALID_DIRECTIVE_DATA, "Invalid directive data (Missing or unmatching quote): %s\n\tat index: %u", data.c_str(), i};
						}
						else
							numOfQuotes++;
					}
					else
					{
						if (numOfQuotes == 0)
							return ErrorMessage{ErrorMessage::INVALID_DIRECTIVE_DATA, "Invalid directive data: %s", data.c_str()};
					}
				}
			}
			else if (directive == ".word" || directive == ".half" || directive == ".byte")
			{
				// TODO: Use IsInteger function here to support the other integer types :D
				//  It is either a comma seperated values or a space seperated ones.
				//  So we will remove all the commas and make sure it is only spaces

				for (uint32_t i = 0; i < data.size(); i++)
				{
					char &c = data[i];
					if (c == ',')
						c = char(27);
				}
				data.erase(std::remove_if(data.begin(), data.end(), [](const char &c)
										  { return c == char(27); }),
						   data.end());

				size_t spaceIndex = data.find(" ");
				size_t lastIndex = 0;
				while (spaceIndex != std::string::npos)
				{
					std::string num = data.substr(lastIndex, spaceIndex - lastIndex);
					if (!IsInteger(num.c_str()))
					{
						return ErrorMessage{ErrorMessage::INVALID_DIRECTIVE_DATA, "Invalid directive data. Number is not integer: %s", num.c_str()};
					}
					lastIndex = spaceIndex + 1;
					spaceIndex = data.find(" ", lastIndex);
				}
			}
			else if (directive == ".space")
			{
				// It is only one number, nothing else
				for (uint32_t i = 0; i < data.size(); i++)
				{
					char &c = data[i];
					if (isdigit(c) || c == 'x' || c == 'b')
						continue;
				}
				data.erase(std::remove_if(data.begin(), data.end(), [](const char &c)
										  { return c == char(27); }),
						   data.end());
				size_t spaceIndex = data.find(" ");
				size_t lastIndex = 0;
				while (spaceIndex != std::string::npos)
				{
					std::string num = data.substr(lastIndex, spaceIndex - lastIndex);
					if (!IsInteger(num.c_str()))
					{
						return ErrorMessage{ErrorMessage::INVALID_DIRECTIVE_DATA, "Invalid directive data. Number is not integer: %s", num.c_str()};
					}
					lastIndex = spaceIndex + 1;
					spaceIndex = data.find(" ", lastIndex);
				}
			}
			else if (directive == ".float" || directive == ".double")
			{
				// It is only one number, nothing else
				for (uint32_t i = 0; i < data.size(); i++)
				{
					char &c = data[i];
					if (isdigit(c) || c == '.' || c == ' ')
						continue;
					else if (c == ',')
						c = char(27);
					else
						return ErrorMessage{ErrorMessage::INVALID_DIRECTIVE_DATA, "Invalid directive data (Found alpha and expected numbers): %s\n\tat index: %u", data.c_str(), i};
				}
				data.erase(std::remove_if(data.begin(), data.end(), [](const char &c)
										  { return c == char(27); }),
						   data.end());
			}
			else
			{
				return ErrorMessage{ErrorMessage::INVALID_DIRECTIVE, "Invalid Directive (Directive not supported): %s", directive.c_str()};
			}

			finalDataSection += directive + "\n" + data + "\n";
			// Structure will be like:
			/*
				<Label>:
				<Directive>
				<Data>
			*/
		}
		dataSection = finalDataSection;
		return ErrorMessage{};
	}

	void Parser::FormatFile()
	{
		this->SanitizeFileContent(m_FileContent, false);
	}

	inline static void GrabLabels(const std::string &section, ParseOutput &output)
	{
		std::stringstream ss;
		ss << section;

		while (!ss.eof())
		{
			std::string line;
			std::getline(ss, line);

			size_t index = line.find(":");

			if (index != std::string::npos)
			{
				// output.labels.push_back(line.substr(0, index));
			}
		}
	}

	template <class T>
	inline T *getArrayFromDataItem(const std::string &data, uint32_t &size, uint32_t floatingPoint = 0)
	{
		size = (uint32_t)std::count(data.begin(), data.end(), ' ') + 1;

		T *arr = new T[size];

		size_t lastIndex = 0;
		size_t space = data.find(' ');
		for (uint32_t i = 0; i < size; i++)
		{
			std::string number = data.substr(lastIndex, space - lastIndex);
			if (floatingPoint == 1)
			{
				arr[i] = (T)std::stof(number);
			}
			else if (floatingPoint == 2)
			{
				arr[i] = (T)std::stod(number);
			}
			else
			{
				arr[i] = (uint32_t)GetInteger(number.c_str(), (uint32_t)number.size());
			}

			lastIndex = space + 1;
			space = data.find(' ', space + 1);
		}
		return arr;
	}

	inline static char *getStringFromDataItem(std::string data, bool nullTerminated)
	{
		for (uint32_t i = 0; i + 1 < data.size(); i++)
		{
			char &c1 = data[i];
			char &c2 = data[i + 1];
			if (c1 == '\\')
			{
				if (c2 == 'n')
				{
					c1 = '\n';
					c2 = char(27);
					i++;
				}
				else if (c2 == '\\')
				{
					c2 = char(27);
					i++;
				}
				else if (c2 == '\"')
				{
					c1 = '\"';
					c2 = char(27);
					i++;
				}
				else if (c2 == '\'')
				{
					c1 = '\'';
					c2 = char(27);
					i++;
				}
				else if (c2 == '\b')
				{
					c1 = '\b';
					c2 = char(27);
					i++;
				}
				else if (c2 == 't')
				{
					c1 = '\t';
					c2 = char(27);
					i++;
				}
				else if (c2 == 'r')
				{
					c1 = '\r';
					c2 = char(27);
					i++;
				}
			}
		}
		data.erase(std::remove_if(data.begin(), data.end(), [](char &c)
								  { return c == char(27); }),
				   data.end());
		char *output = nullptr;
		if (nullTerminated)
		{
			output = new char[data.size() + 1];
			output[data.size()] = '\0';
		}
		else
		{
			output = new char[data.size()];
		}

		for (uint32_t i = 0; i < data.size(); i++)
			output[i] = data[i];

		return output;
	}

	inline static void GrabDataSectionItems(const std::string &dataSection, ParseOutput &output)
	{
		std::stringstream ss;
		ss << dataSection;

		bool foundDataSection = false;
		uint32_t dataSectionStart = 0; // to be added later to be the start of the memory
		while (!ss.eof())
		{
			std::string label, type, data;
			std::getline(ss, label);
			std::getline(ss, type);
			std::getline(ss, data);
			if (label.size() < 2)
				break;
			label.pop_back();
			uint32_t size = 0;
			uint32_t multiplier = 1;
			if (type == ".ascii")
			{
				data.pop_back();
				data.erase(data.begin());
				size = (uint32_t)data.size();
				void *finalData = (void *)getStringFromDataItem(data, false);
				output.dataSection.AddItem(label, RISC::DataItem::STRING, size, finalData);
				multiplier = 1;
			}
			else if (type == ".string" || type == ".asciz")
			{
				data.pop_back();
				data.erase(data.begin());
				size = (uint32_t)data.size() + 1;
				void *finalData = (void *)getStringFromDataItem(data, true);
				output.dataSection.AddItem(label, RISC::DataItem::C_STRING, size, finalData);
				multiplier = 1;
			}
			else if (type == ".word")
			{
				void *finalData = (void *)getArrayFromDataItem<uint32_t>(data, size);
				output.dataSection.AddItem(label, RISC::DataItem::WORD, size, finalData);
				multiplier = 4;
			}
			else if (type == ".half")
			{
				void *finalData = (void *)getArrayFromDataItem<uint16_t>(data, size);
				output.dataSection.AddItem(label, RISC::DataItem::HALF_WORD, size, finalData);
				multiplier = 2;
			}
			else if (type == ".byte")
			{
				void *finalData = (void *)getArrayFromDataItem<uint8_t>(data, size);
				output.dataSection.AddItem(label, RISC::DataItem::BYTE, size, finalData);
				multiplier = 1;
			}
			else if (type == ".double")
			{
				void *finalData = (void *)getArrayFromDataItem<double>(data, size, 2);
				output.dataSection.AddItem(label, RISC::DataItem::DOUBLE, size, finalData);
				multiplier = 8;
			}
			else if (type == ".float")
			{
				void *finalData = (void *)getArrayFromDataItem<float>(data, size, 1);
				output.dataSection.AddItem(label, RISC::DataItem::FLOAT, size, finalData);
				multiplier = 4;
			}
			else if (type == ".space")
			{
				void *finalData = (void *)getArrayFromDataItem<uint32_t>(data, size);
				output.dataSection.AddItem(label, RISC::DataItem::SPACE, size, finalData);
				multiplier = 4;
			}
			output.dataSectionSize += (size * multiplier);
		}
	}

	inline static void GrabInstructions(const std::string &textSection, ParseOutput &output)
	{
		using namespace RISC;

		std::stringstream ss;
		ss << textSection;
		uint32_t lineNumber = 0;
		auto &instTypes = Instruction::typeNames;

		while (!ss.eof())
		{
			std::string line;
			std::getline(ss, line);
			if (line.size() < 4)
				continue;

			for (auto &c : line)
				c = toupper(c);

			lineNumber++;
			size_t index = line.find(" ");
			bool err = false;
			std::string name = line.substr(0, index);
			auto it = std::find(instTypes.begin(), instTypes.end(), name);

			if (it != instTypes.end())
			{
				Instruction inst(name);
				inst.lineNumber = lineNumber;
				auto type = inst.writeType;
				char rd[8] = {0}, rs1[8] = {0}, rs2[8] = {0}, imm[16] = {0};
				switch (type)
				{
				case TYPE1: // inst rd, imm
				{
					int n = sscanf(line.c_str(), "%*[A-Z] %[A-Za-z0-9], %[A-Za-z0-9-]", rd, imm);
					if (n < 2)
					{
						output.err = {ErrorMessage::INVALID_PARAMETERS, "Expected format <Instruction> <rd>, <imm>; Instruction: %s", name.c_str()};
						return;
					}
					std::string rdString = RegisterFile::GetTrueRegName(rd);
					if (rdString == "INVALID" || !IsInteger(imm))
					{
						output.err = {ErrorMessage::INVALID_PARAMETERS, "Invalid format, instruction: %s", name.c_str()};
						return;
					}
					uint32_t finalRD;
					sscanf(rdString.c_str(), "%*[Xx]%d", &finalRD);

					uint32_t finalIMM = GetInteger(imm, 16);
					inst.imm = finalIMM;
					inst.rd = finalRD;
					output.instructions.push_back(inst);
				}
				break;
				case TYPE2: // inst rd, imm(rs1)
				{
					int n = sscanf(line.c_str(), "%*[A-Z] %[A-Za-z0-9], %[A-Za-z0-9-](%[A-Za-z0-9])", rd, imm, rs1);
					if (n < 3)
					{
						output.err = {ErrorMessage::INVALID_PARAMETERS, "Expected format <Instruction> <rd>, <imm>(<rs1>); Instruction: %s", name.c_str()};
						return;
					}
					std::string rdString = RegisterFile::GetTrueRegName(rd);
					std::string rs1String = RegisterFile::GetTrueRegName(rs1);
					if (rdString == "INVALID" || rs1String == "INVALID" || !IsInteger(imm))
					{
						output.err = {ErrorMessage::INVALID_PARAMETERS, "Invalid format, instruction: %s", name.c_str()};
						return;
					}
					uint32_t finalRD, finalRS1;
					sscanf(rdString.c_str(), "%*[Xx]%d", &finalRD);
					sscanf(rs1String.c_str(), "%*[Xx]%d", &finalRS1);

					uint32_t finalIMM = GetInteger(imm, 16);
					inst.imm = finalIMM;
					inst.rd = finalRD;
					inst.rs1 = finalRS1;
					output.instructions.push_back(inst);
				}
				break;
				case TYPE3: // inst rs2, imm(rs1)
				{
					int n = sscanf(line.c_str(), "%*[A-Z] %[A-Za-z0-9], %[A-Za-z0-9-](%[A-Za-z0-9])", rs2, imm, rs1);
					if (n < 3)
					{
						output.err = {ErrorMessage::INVALID_PARAMETERS, "Expected format <Instruction> <rs2>, <imm>(<rs1>); Instruction: %s", name.c_str()};
						return;
					}
					std::string rs1String = RegisterFile::GetTrueRegName(rs1);
					std::string rs2String = RegisterFile::GetTrueRegName(rs2);
					if (rs1String == "INVALID" || rs2String == "INVALID" || !IsInteger(imm))
					{
						output.err = {ErrorMessage::INVALID_PARAMETERS, "Invalid format, instruction: %s", name.c_str()};
						return;
					}
					uint32_t finalRS1, finalRS2;
					sscanf(rs1String.c_str(), "%*[Xx]%d", &finalRS1);
					sscanf(rs2String.c_str(), "%*[Xx]%d", &finalRS2);
					uint32_t finalIMM = GetInteger(imm, 16);

					inst.rs1 = finalRS1;
					inst.rs2 = finalRS2;
					inst.imm = finalIMM;
					output.instructions.push_back(inst);
				}
				break;
				case TYPE4: // inst rd, rs1, rs2
				{
					int n = sscanf(line.c_str(), "%*[A-Z] %[A-Za-z0-9], %[A-Za-z0-9], %[A-Za-z0-9]", rd, rs1, rs2);
					if (n < 3)
					{
						output.err = {ErrorMessage::INVALID_PARAMETERS, "Expected format <Instruction> <rd>, <rs1>, <rs2>; Instruction: %s", name.c_str()};
						return;
					}
					std::string rdString = RegisterFile::GetTrueRegName(rd);
					std::string rs1String = RegisterFile::GetTrueRegName(rs1);
					std::string rs2String = RegisterFile::GetTrueRegName(rs2);
					if (rs2String == "INVALID" || rs1String == "INVALID" || rdString == "INVALID")
					{
						output.err = {ErrorMessage::INVALID_PARAMETERS, "Invalid format, instruction: %s", name.c_str()};
						return;
					}
					uint32_t finalRD, finalRS1, finalRS2;
					sscanf(rdString.c_str(), "%*[Xx]%d", &finalRD);
					sscanf(rs1String.c_str(), "%*[Xx]%d", &finalRS1);
					sscanf(rs2String.c_str(), "%*[Xx]%d", &finalRS2);

					inst.rd = finalRD;
					inst.rs1 = finalRS1;
					inst.rs2 = finalRS2;
					output.instructions.push_back(inst);
				}
				break;
				case TYPE5: // inst rd, rs1, imm
				{
					int n = sscanf(line.c_str(), "%*[A-Z] %[A-Za-z0-9], %[A-Za-z0-9], %[A-Za-z0-9-]", rd, rs1, imm);
					if (n < 3)
					{
						output.err = ErrorMessage(ErrorMessage::INVALID_PARAMETERS, "Expected format <Instruction> <rd>, <rs1>, <imm>; Instruction: %s", name.c_str());
						return;
					}
					std::string rdString = RegisterFile::GetTrueRegName(rd);
					std::string rs1String = RegisterFile::GetTrueRegName(rs1);
					if (rs1String == "INVALID" || rdString == "INVALID" || !IsInteger(imm))
					{
						output.err = {ErrorMessage::INVALID_PARAMETERS, "Invalid format, instruction: %s", name.c_str()};
						return;
					}
					uint32_t finalRD, finalRS1, finalIMM;

					sscanf(rdString.c_str(), "%*[Xx]%d", &finalRD);
					sscanf(rs1String.c_str(), "%*[Xx]%d", &finalRS1);
					finalIMM = GetInteger(imm, 16);

					inst.rd = finalRD;
					inst.rs1 = finalRS1;
					inst.imm = finalIMM;
					output.instructions.push_back(inst);
				}
				break;
				case TYPE6: // ecall / ebreak
				{
					output.instructions.push_back(inst);
				}
				break;
				case TYPE7: // inst rs1, rs2, imm
				{
					int n = sscanf(line.c_str(), "%*[A-Z] %[A-Za-z0-9], %[A-Za-z0-9], %[A-Za-z0-9-]", rs1, rs2, imm);
					if (n < 3)
					{
						output.err = {ErrorMessage::INVALID_PARAMETERS, "Expected format <Instruction> <rs1>, <rs2>, <imm/Label>; Instruction: %s", name.c_str()};
						return;
					}
					std::string rs1String = RegisterFile::GetTrueRegName(rs1);
					std::string rs2String = RegisterFile::GetTrueRegName(rs2);
					if (rs1String == "INVALID" || rs2String == "INVALID" || !IsInteger(imm))
					{
						output.err = {ErrorMessage::INVALID_PARAMETERS, "Invalid format, instruction: %s", name.c_str()};
						return;
					}
					uint32_t finalRS2, finalRS1, finalIMM;

					sscanf(rs1String.c_str(), "%*[Xx]%d", &finalRS1);
					sscanf(rs2String.c_str(), "%*[Xx]%d", &finalRS2);
					finalIMM = GetInteger(imm, 16);

					inst.rs1 = finalRS1;
					inst.rs2 = finalRS2;
					inst.imm = finalIMM;
					output.instructions.push_back(inst);
				}
				break;
				default: // INVALID
					output.err = {ErrorMessage::INVALID_INSTRUCTION, "Unsupported instruction: Did you write it correctly?\n\t %s", inst.instName.c_str()};
					return;
					break;
				}
			}
			else
				output.err = {ErrorMessage::INVALID_INSTRUCTION, "Unsupported instruction: Did you write it correctly?\n\t %s", line.c_str()};
		}
	}

	Parser::FileText Parser::GenerateTextAndData(const std::string &fileContent)
	{
		Parser::FileText output;
		size_t startOfDataSection = fileContent.find(".data");
		{
			size_t endOfDataIndex = fileContent.find(".data", startOfDataSection + 5);
			// The bold assumption that the data section exists only ONCE
			if (endOfDataIndex != std::string::npos)
			{
				output.err = {ErrorMessage::INVALID_STRUCTURE_DATA, "There is more than one data section. (There should be only one data section)"};
				return output;
			}
		}
		size_t startOfTextSection = fileContent.find(".text");
		{
			size_t endOfTextSection = fileContent.find(".text", startOfTextSection + 5);
			// We will always have a text section
			if (startOfTextSection == std::string::npos || endOfTextSection != std::string::npos)
			{
				output.err = {ErrorMessage::INVALID_STRUCTURE_TEXT, "There is more than one text section. (There should be only one textx section"};
				return output;
			}
		}

		// we will not necc. have a data section
		if (startOfDataSection != std::string::npos)
		{
			auto &dataSection = output.dataSection;
			if (startOfDataSection < startOfTextSection)
			{
				size_t count = startOfTextSection - 6;
				size_t start = startOfDataSection + 6;
				dataSection = fileContent.substr(start, count);
			}
			else
			{
				size_t start = startOfDataSection + 6;
				dataSection = fileContent.substr(start);
			}
		}

		{
			auto &textSection = output.textSection;
			if (startOfTextSection < startOfDataSection)
			{
				size_t count = startOfDataSection - 6;
				size_t start = startOfTextSection + 6;
				textSection = fileContent.substr(start, count);
			}
			else
			{
				size_t start = startOfTextSection + 6;
				textSection = fileContent.substr(start);
			}
		}
		return output;
	}

	ParseOutput Parser::Parse()
	{
		ParseOutput output;
		SanitizeFileContent(m_ParsableFile, true);
		auto files = GenerateTextAndData(m_ParsableFile);
		if (files.err)
		{
			std::cerr << "Generation err: " << files.err << std::endl;
			exit(1);
		}
		auto err = SanitizeDataSection(files.dataSection);
		if (err)
		{
			std::cerr << "err: " << err << std::endl;
			exit(1);
		}
		GrabDataSectionItems(files.dataSection, output);
		GrabInstructions(files.textSection, output);

		return output;
	}
}