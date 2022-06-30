#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <map>
#include <fstream>
#include "Parser/Parser.h"

int main(int argc, const char *argv[])
{
	const std::string filePath = std::string(RESOURCES_DIR) + std::string("/testfile.txt");
	std::cout << filePath << std::endl;
	Parser::Parser parser;

	if (parser.ReadFile(filePath))
	{
		parser.Parse();
	}
	else
	{
		std::cout << "Was not able to open the file\n";
	}

	return 0;
}