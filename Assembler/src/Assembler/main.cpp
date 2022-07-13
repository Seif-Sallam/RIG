#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <bitset>
#include <iomanip>

#include <Parser/Parser.h>
#include <RISC/Translator.h>

int main(int argc, char const *argv[])
{
	if (argc < 3 || (!isdigit(argv[1][0])))
	{
		std::cerr << "Invalid number of arguments\n\tUsage:\t./assembler <Output Option> <assembly_file> [... list of files to assemble]\n"
					 "Output Options are either\n\t0: ASCII Biniary\n\t1: Binary\n\t2: Hexadecimal";
		exit(1);
	}

	uint32_t mode = argv[1][0] - '0';

	RISC::Translator translator;
	for (uint32_t i = 2; i < argc; i++)
	{
		Parser::Parser parser;
		bool success = parser.ReadFile(argv[i]);
		if (!success)
		{
			std::cerr << "failed to open the file: " << argv[i] << std::endl;
			continue;
		}

		auto parseOutput = parser.Parse();
		if (parseOutput.err)
		{
			std::cerr << "Failed to parse the file: " << argv[i] << "\nerr: " << parseOutput.err << std::endl;
			continue;
		}
		std::ofstream outputFile;
		std::string title = argv[i];
		{
			uint32_t index = title.find(".");
			if (index != std::string::npos)
			{
				title.erase(title.begin() + index, title.end());
				if (mode == 0)
					title += "_ascii_binary.txt";
				else if (mode == 1)
					title += "_binary.bin";
				else if (mode == 2)
					title += "_hex.txt";
			}
		}
		union Assembled
		{
			uint8_t asChar[4];
			uint32_t asInt;
		};
		outputFile.open(title);
		for (auto &inst : parseOutput.instructions)
		{
			Assembled assembledInst;
			assembledInst.asInt = translator.Assemble(inst);
			if (mode == 0)
			{
				std::bitset<32> out = assembledInst.asInt;
				outputFile << out << '\n';
			}
			else if (mode == 1)
			{
				for (int j = 0; j < 4; j++)
					outputFile << assembledInst.asChar[j];
			}
			else if (mode == 2)
			{
				outputFile << "0x" << std::setfill('0') << std::setw(8) << std::hex << assembledInst.asInt << '\n';
			}
		}
		outputFile.close();
		std::cout << "Generated file: " << title << std::endl;
	}
	return 0;
}
