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

#include "Utils/Logger.h"

typedef Util::Logger Log;
int main(int argc, const char *argv[])
{
	Log::Debug("Hello, World! {}", "Debug");
	Log::Info("Hello, World! {}", "Info");
	Log::Warning("Hello, World! {}", "Warning");
	Log::Error("Hello, World! {}", "Error");
	Log::Success("Hello, World! {}", "Success");

	if (true)
	{
		const std::string filePath = std::string(RESOURCES_DIR) + std::string("/testfile.txt");
		Parser::Parser parser;
		if (parser.ReadFile(filePath))
		{
			auto parsedData = parser.Parse();
			if (parsedData.err)
			{
				Log::Error("Parsed data error: {}", parsedData.err.msg);
				return 1;
			}
			Log::Info("Data Section:");
			parsedData.dataSection.Print();

			Log::Info("Instructions:");
			for (auto &inst : parsedData.instructions)
			{
				std::string fmt = RISC::Instruction::FormatInstruction(inst);
				Log::Print("{}", fmt);
			}
		}
		else
		{
			Log::Error("Was not able to open the file");
		}
	}
	else
	{
	}
	IDE::MainWindow mainWindow(argc, argv);

	mainWindow.Run();
	return 0;
}