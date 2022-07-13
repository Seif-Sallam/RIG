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
	const std::string filePath = std::string(RESOURCES_DIR) + std::string("testfile.txt");
	Parser::Parser parser;
	if (parser.ReadFile(filePath))
	{
		auto parsedData = parser.Parse();
		if (parsedData.err)
		{
			std::cout << "ERROR: " << parsedData.err << std::endl;
			return 1;
		}
		std::cout << "INSTRUCTIONS:\n";
		for (auto &inst : parsedData.instructions)
		{
			std::string fmt = RISC::Instruction::FormatInstruction(inst);

			std::cout << fmt << std::endl;
		}
	}
	else
	{
		std::cout << "Was not able to open the file\n";
	}

	return 0;
}