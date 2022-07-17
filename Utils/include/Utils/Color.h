#pragma once

#include <string>
#include <string_view>

#include <fmt/core.h>

#ifdef _WIN32
#include <windows.h>

template <class... ArgsType>
std::string PrintWithColor(std::string_view color, std::string_view linefmt, ArgsType &&...args)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	static constexpr std::string_view colors[] = {"white", "blue", "green", "cyan", "red", "magenta", "yellow"};
	static constexpr size_t numOfColors = sizeof(colors) / sizeof(*colors);
	int col = 7;
	for (size_t i = 0; i < numOfColors; i++)
		if (color == colors[i])
		{
			col = i - 1;
			break;
		}
	if (col == 0)
		col = 7;
	std::string msg = fmt::format(linefmt, std::forward<ArgsType>(args)...);
	SetConsoleTextAttribute(hConsole, col);
	fmt::print("{}\n", msg);
	SetConsoleTextAttribute(hConsole, 7);

	return msg;
}

#else

template <class... ArgsType>
std::string PrintWithColor(std::string_view color, std::string_view linefmt, ArgsType &&...args)
{
	static constexpr std::string_view colors[] = {"white", "blue", "green", "cyan", "red", "magenta", "yellow"};
	static constexpr std::string_view colorCodes[] = {"\033[0m", "\033[0;34m",
													  "\033[0;32m", "\033[0;36m", "\033[0;31m", "\033[0;35m", "\033[0;33m", "\033[0m"};
	static constexpr size_t numOfColors = sizeof(colors) / sizeof(*colors);
	const std::string_view *col = &colorCodes[numOfColors];
	static const std::string_view *noColor = &colorCodes[numOfColors];
	for (size_t i = 0; i < numOfColors; i++)
		if (color == colors[i])
		{
			col = &colorCodes[i];
			break;
		}
	std::string msg = fmt::format(linefmt, std::forward<ArgsType>(args)...);

	fmt::print("{}{}{}\n", *col, msg, *noColor);
	return msg;
}

#endif
