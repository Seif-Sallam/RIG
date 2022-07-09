#include <fstream>
#include <sstream>
#include <math.h>
#include <cstring>
#include <algorithm>
#include <regex>

#include <Parser/Parser.h>

#include "RISC/Instruction.h"
#include "RISC/RegisterFile.h"

namespace Parser
{
	Parser::Parser()
		: m_FileContent("")
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

			auto it = std::remove(m_FileContent.begin(), m_FileContent.end(), '\r');
			m_FileContent.erase(it, m_FileContent.end());
			m_ParsableFile = m_FileContent;
			SanitizeFileContent(m_ParsableFile, true);
			std::cout << m_ParsableFile << std::endl;
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
		std::cout << m_FileContent << std::endl;
	}

	Parser::~Parser()
	{
		CleanParserData();
	}

	inline static void AddNewLineAfter(std::string &str, const char *pattern)
	{
		int32_t findIndex = str.find(pattern);
		if (findIndex != std::string::npos)
		{
			if (findIndex - str.size() < 10)
			{
				uint32_t index = findIndex + strlen(pattern);
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

					{
					}
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
				if (fonudNewLine && newLineCount >= (2 + !removeComments))
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
		fileContent.erase(it, fileContent.end());
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
			uint32_t spaceIndex = directive.find(" ");
			data = directive.substr(spaceIndex + 1);
			directive = directive.substr(0, spaceIndex);

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
				// make sure the data is correct
			}
			else if (directive == ".word" || directive == ".half" || directive == ".byte")
			{
			}
			else if (directive == ".space")
			{
			}
			else if (directive == ".float" || directive == ".double")
			{
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
				Data
			*/
		}
		dataSection = finalDataSection;
		return ErrorMessage{ErrorMessage::NO_ERROR, "NO ERR"};
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

			int32_t index = line.find(":");

			if (index != std::string::npos)
			{
				// output.labels.push_back(line.substr(0, index));
			}
		}
	}

	inline static void GrabDataSectionItems(const std::string &dataSection, ParseOutput &output)
	{
		std::stringstream ss;
		ss << dataSection;

		bool foundDataSection = false;
		uint32_t dataSectionStart = 0; // to be added later to be the start of the memory
		while (!ss.eof())
		{
			std::string line;
			std::getline(ss, line);
			// to be implemented after implementing the directives class (data section of the program)
			if (uint32_t index = line.find(":"); index != std::string::npos)
			{
				std::string label = line.substr(0, index);
				if (std::regex_match(label, std::regex("([a-zA-Z_][a-zA-Z0-9_]*)")))
				{
					DataLabel dataLbl;
					dataLbl.labelString = label;
					std::string dataLine;
					std::getline(ss, dataLine);
					if (uint32_t dotIndex = dataLine.find("."); dotIndex != std::string::npos)
					{
						uint32_t spaceIndex = 0;
						for (uint32_t i = dotIndex + 1; i < dataLine.size(); i++)
							if (dataLine[i] == ' ')
							{
								spaceIndex = i;
								break;
							}
						if (spaceIndex == 0)
						{
							output.err = {ErrorMessage::INVALID_DIRECTIVE, "Directive was not specified: %s", dataLine};
							return;
						}
						std::string directive = dataLine.substr(dotIndex + 1, spaceIndex - dotIndex);
						std::string directiveData = dataLine.substr(spaceIndex, -1);
						if (directive == "asciz" || directive == "string")
						{
							if (std::regex_match(directiveData, std::regex("(\"[\\s\\S]*\")")))
							{
								dataLbl.data = directiveData;
							}
							else
							{
								output.err = {ErrorMessage::INVALID_DIRECTIVE, "Directive was not specified: %s", dataLine};
								return;
							}
						}
						else if (directive == "word")
						{
							for (auto &c : directiveData)
							{
								if (!isdigit(c) && c != ' ')
								{
									output.err = {ErrorMessage::INVALID_DIRECTIVE, "Directive was not specified: %s", dataLine};
									return;
								}
							}
							dataLbl.data = directiveData;
						}
						else if (directive == "byte")
						{
						}
						else if (directive == "half")
						{
						}
						else if (directive == "ascii")
						{
						}
						else if (directive == "space")
						{
						}
						else if (directive == "double")
						{
						}
						else if (directive == "float")
						{
						}
						else
						{
							output.err = {ErrorMessage::INVALID_DIRECTIVE, "Directive was not specified: %s", dataLine};
							return;
						}
					}
					else
					{
						output.err = {ErrorMessage::INVALID_DIRECTIVE, "Directive was not specified: %s", dataLine};
						return;
					}
					output.dataLabels.push_back(dataLbl);
				}
				else
				{
					output.err = {ErrorMessage::INVALID_LABEL, "Invalid Label: %s", label};
					return;
				}
			}
			else
			{
				// There is an error in the string.
				continue;
			}
		}
	}

	inline static bool IsInteger(char *value)
	{
		bool hex = false;
		bool isBinary = false;

		if (value[0] == '0' && tolower(value[1]) == 'x')
			hex = true;
		if (value[0] == '0' && tolower(value[1]) == 'b')
			isBinary = true;

		uint32_t start = (hex || isBinary) ? 2 : 0;

		for (auto p = value + 2; *p != '\0'; p++)
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

	inline static uint32_t GetInteger(char *imm, uint32_t size)
	{
		uint32_t sum = 0;
		uint32_t base = 10;
		uint32_t pos = 0;
		bool isHex = false;
		bool isBinary = false;
		if (imm[0] == '0' && tolower(imm[1]) == 'x')
		{
			base = 16;
			isHex = true;
		}
		if (imm[0] == '0' && tolower(imm[1]) == 'b')
		{
			base = 2;
			isBinary = true;
		}
		int32_t start = (isHex || isBinary) ? 2 : 0;
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

			sum += value * pow(base, pos);
			pos++;
		}

		return sum;
	}

	inline static void GrabInstructions(const std::string &textSection, ParseOutput &output)
	{
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
			uint32_t index = line.find(" ");
			bool err = false;
			std::string name = line.substr(0, index);
			auto it = std::find(instTypes.begin(), instTypes.end(), name);

			if (it != instTypes.end())
			{
				Instruction inst(name);
				inst.lineNumber = lineNumber;
				auto type = inst.WriteType();
				char rd[8] = {0}, rs1[8] = {0}, rs2[8] = {0}, imm[16] = {0};
				switch (type)
				{
				case TYPE1: // inst rd, imm
				{
					int n = sscanf(line.c_str(), "%*[A-Z] %[A-Za-z0-9], %[A-Za-z0-9]", rd, imm);
					if (n < 2)
					{
						output.err = {ErrorMessage::INVALID_PARAMETERS, "Expected format <Instruction> <rd>, <imm>; Instruction: %s", name};
						return;
					}
					std::string rdString = RegisterFile::GetTrueRegName(rd);
					if (rdString == "INVALID" || !IsInteger(imm))
					{
						output.err = {ErrorMessage::INVALID_PARAMETERS, "Invalid format, instruction: %s", name};
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
					int n = sscanf(line.c_str(), "%*[A-Z] %[A-Za-z0-9], %[A-Za-z0-9](%[A-Za-z0-9])", rd, imm, rs1);
					if (n < 3)
					{
						output.err = {ErrorMessage::INVALID_PARAMETERS, "Expected format <Instruction> <rd>, <imm>(<rs1>); Instruction: %s", name};
						return;
					}
					std::string rdString = RegisterFile::GetTrueRegName(rd);
					std::string rs1String = RegisterFile::GetTrueRegName(rs1);
					if (rdString == "INVALID" || rs1String == "INVALID" || !IsInteger(imm))
					{
						output.err = {ErrorMessage::INVALID_PARAMETERS, "Invalid format, instruction: %s", name};
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
					int n = sscanf(line.c_str(), "%*[A-Z] %[A-Za-z0-9], %[A-Za-z0-9](%[A-Za-z0-9])", rs2, imm, rs1);
					if (n < 3)
					{
						output.err = {ErrorMessage::INVALID_PARAMETERS, "Expected format <Instruction> <rs2>, <imm>(<rs1>); Instruction: %s", name};
						return;
					}
					std::string rs1String = RegisterFile::GetTrueRegName(rs1);
					std::string rs2String = RegisterFile::GetTrueRegName(rs2);
					if (rs1String == "INVALID" || rs2String == "INVALID" || !IsInteger(imm))
					{
						output.err = {ErrorMessage::INVALID_PARAMETERS, "Invalid format, instruction: %s", name};
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
						output.err = {ErrorMessage::INVALID_PARAMETERS, "Expected format <Instruction> <rd>, <rs1>, <rs2>; Instruction: %s", name};
						return;
					}
					std::string rdString = RegisterFile::GetTrueRegName(rd);
					std::string rs1String = RegisterFile::GetTrueRegName(rs1);
					std::string rs2String = RegisterFile::GetTrueRegName(rs2);
					if (rs2String == "INVALID" || rs1String == "INVALID" || rdString == "INVALID")
					{
						output.err = {ErrorMessage::INVALID_PARAMETERS, "Invalid format, instruction: %s", name};
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
					int n = sscanf(line.c_str(), "%*[A-Z] %[A-Za-z0-9], %[A-Za-z0-9], %[A-Za-z0-9]", rd, rs1, imm);
					if (n < 3)
					{
						output.err = ErrorMessage(ErrorMessage::INVALID_PARAMETERS, "Expected format <Instruction> <rd>, <rs1>, <imm>; Instruction: %s", name.c_str());
						return;
					}
					std::string rdString = RegisterFile::GetTrueRegName(rd);
					std::string rs1String = RegisterFile::GetTrueRegName(rs1);
					if (rs1String == "INVALID" || rdString == "INVALID" || !IsInteger(imm))
					{
						output.err = {ErrorMessage::INVALID_PARAMETERS, "Invalid format, instruction: %s", name};
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
				case TYPE6: // ecall / fence / ebreak
				{
					output.instructions.push_back(inst);
				}
				break;
				case TYPE7: // inst rs1, rs2, imm
				{
					int n = sscanf(line.c_str(), "%*[A-Z] %[A-Za-z0-9], %[A-Za-z0-9], %[A-Za-z0-9]", rs1, rs2, imm);
					if (n < 3)
					{
						output.err = {ErrorMessage::INVALID_PARAMETERS, "Expected format <Instruction> <rs1>, <rs2>, <imm/Label>; Instruction: %s", name};
						return;
					}
					std::string rs1String = RegisterFile::GetTrueRegName(rs1);
					std::string rs2String = RegisterFile::GetTrueRegName(rs2);
					if (rs1String == "INVALID" || rs2String == "INVALID" || !IsInteger(imm))
					{
						output.err = {ErrorMessage::INVALID_PARAMETERS, "Invalid format, instruction: %s", name};
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
					output.err = {ErrorMessage::INVALID_INSTRUCTION, "Unsupported instruction: Did you write it correctly?\n\t %s", inst};
					return;
					break;
				}
			}
			else
				output.err = {ErrorMessage::INVALID_INSTRUCTION, "Unsupported instruction: Did you write it correctly?\n\t %s", line};
		}
	}

	Parser::FileText Parser::GenerateTextAndData(const std::string &fileContent)
	{
		std::cout << "Parsing text and data\n";
		Parser::FileText output;
		int32_t startOfDataSection = fileContent.find(".data");
		{
			int32_t endOfDataIndex = fileContent.find(".data", startOfDataSection + 5);
			// The bold assumption that the data section exists only ONCE
			if (endOfDataIndex != std::string::npos)
			{
				printf("startOfDataSection: %u, end: %u\n", startOfDataSection, endOfDataIndex);
				output.err = {ErrorMessage::INVALID_STRUCTURE_DATA, "There is more than one data section. (There should be only one data section)"};
				return output;
			}
		}
		int32_t startOfTextSection = fileContent.find(".text");
		{
			int32_t endOfTextSection = fileContent.find(".text", startOfTextSection + 5);
			// We will always have a text section
			if (startOfTextSection == std::string::npos || endOfTextSection != std::string::npos)
			{
				printf("startOfTextSection: %d, end: %d\n", startOfTextSection, endOfTextSection);
				output.err = {ErrorMessage::INVALID_STRUCTURE_TEXT, "There is more than one text section. (There should beo nly one data section"};
				return output;
			}
		}

		// we will not necc. have a data section
		if (startOfDataSection != std::string::npos)
		{
			auto &dataSection = output.dataSection;
			if (startOfDataSection < startOfTextSection)
			{
				uint32_t count = startOfTextSection - 6;
				uint32_t start = startOfDataSection + 6;
				dataSection = fileContent.substr(start, count);
			}
			else
			{
				uint32_t start = startOfDataSection + 6;
				dataSection = fileContent.substr(start);
			}
		}

		{
			auto &textSection = output.textSection;
			if (startOfTextSection < startOfDataSection)
			{
				uint32_t count = startOfDataSection - 6;
				uint32_t start = startOfTextSection + 6;
				textSection = fileContent.substr(start, count);
			}
			else
			{
				uint32_t start = startOfTextSection + 6;
				textSection = fileContent.substr(start);
			}
		}
		std::cout << "Finished\n";
		return output;
	}

	ParseOutput Parser::Parse()
	{
		ParseOutput output;

		auto files = GenerateTextAndData(m_ParsableFile);
		std::cout << "Starting sanitization\n";
		// GrabLabels(m_ParsableFile, output);
		auto err = SanitizeDataSection(files.dataSection);
		std::cout << files.dataSection << std::endl;
		if (err)
		{
			std::cout << "err: " << err << std::endl;
			exit(0);
		}
		// GrabDataSectionItems(files.dataSection, output);
		// GrabInstructions(files.textSection, output);
		// std::cout << "LABELS:\n";
		// for (auto &label : output.labels)
		// {
		// 	std::cout << label << "\n";
		// }
		// std::cout << "\n";
		return output;
	}

	const std::string g_Delimnators = ":#().,";
}