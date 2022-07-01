#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <map>
#include <fstream>
#include "Parser/Parser.h"
#include "RISC/Instruction.h"

int main(int argc, const char *argv[])
{
	const std::string filePath = std::string(RESOURCES_DIR) + std::string("/testfile.txt");
	std::cout << filePath << std::endl;
	Parser::Parser parser;
	std::cout << "Parsable Data:\n";
	if (parser.ReadFile(filePath))
	{
		std::cout << "\nFormatted Data:\n\n";
		parser.FormatFile();
		parser.PrintFileContent();
		std::cout << "\n";
		auto parsedData = parser.Parse();
		if (parsedData.err)
		{
			std::cout << "ERROR: " << parsedData.err << std::endl;
			return 1;
		}
		std::cout << "INSTRUCTIONS:\n";
		for (auto &inst : parsedData.instructions)
		{
			std::string fmt = Instruction::FormatInstruction(inst);

			std::cout << fmt << std::endl;
		}
	}
	else
	{
		std::cout << "Was not able to open the file\n";
	}

	return 0;
}