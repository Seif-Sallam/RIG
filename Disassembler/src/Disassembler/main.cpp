#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include <RISC/Translator.h>
#include <Utils/Logger.h>

typedef Util::Logger Log;

int main(int argc, char const *argv[])
{
	if (argc < 2)
	{
		std::cerr << "Invalid number of arguments\n"
					 "\tExpected: <file_name> [... file names]\n";
		return 1;
	}

	RISC::Translator translator;

	for (int i = 1; i < argc; i++)
	{
		std::vector<RISC::Instruction> instructions;
		std::ifstream inputFile;
		inputFile.open(argv[i], std::ios::ate | std::ios::binary);
		{
			if (!inputFile.is_open())
			{
				Log::Error("Failed to open the file {}", argv[i]);
				continue;
			}
			Log::Info("Reading: {}", argv[i]);

			size_t fileSize = inputFile.tellg();
			inputFile.seekg(std::ios::beg);
			char *fileContent = new char[fileSize];

			inputFile.read(fileContent, fileSize);
			for (size_t i = 0; i < fileSize; i += 4)
			{
				uint32_t instBinary = (unsigned char)fileContent[i] | (unsigned char)fileContent[i + 1] << 8 | (unsigned char)fileContent[i + 2] << 16 | (unsigned char)fileContent[i + 3] << 24;
				instructions.push_back(translator.Dissassemble(instBinary));
			}
			delete[] fileContent;
		}
		inputFile.close();

		std::ofstream outputFile;
		std::string title = argv[i];
		{
			size_t index = title.find(".");
			if (index != std::string::npos)
			{
				title.erase(title.begin() + index, title.end());
				title += "_diss.asm";
			}
		}
		outputFile.open(title);
		Log::Info(".text");
		outputFile << ".text\n";
		for (auto &inst : instructions)
		{
			auto str = RISC::Instruction::FormatInstruction(inst);
			Log::Info("\t{}", str);
			outputFile << str << std::endl;
		}
		outputFile.close();

		Log::Success("Generated File: {}", title);
	}
	return 0;
}
