#include "Utils/Logger.h"
#include "Utils/Color.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

namespace Utils
{
	std::vector<std::pair<std::string, Logger::MetaData>> Logger::buffer{};
}