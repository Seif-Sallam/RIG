#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <map>
#include <fstream>

#include "Parser/Parser.h"

#include "RISC/Instruction.h"

#include "IDE/MainWindow.h"
int main(int argc, const char *argv[])
{
	if (false)
	{

		const std::string filePath = std::string(RESOURCES_DIR) + std::string("/testfile.txt");
		Parser::Parser parser;
		if (parser.ReadFile(filePath))
		{
			auto parsedData = parser.Parse();
			if (parsedData.err)
			{
				std::cout << "ERROR: " << parsedData.err << std::endl;
				return 1;
			}
			std::cout << "Data Section: \n";
			parsedData.dataSection.Print();

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
	}
	else
	{
		IDE::MainWindow mainWindow(argc, argv);

		mainWindow.Run();
	}
	return 0;
}