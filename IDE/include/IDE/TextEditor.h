#pragma once

#include <imgui_internal.h>
#include <imgui.h>

#include <string>
#include <Utils/Logger.h>

// namespace IDE
// {
// class TextEditor
// {/
// 	struct Coordinate
// 	{
// 		int32_t line, column;
// 		inline Coordinate() : line(0), column(0) {}
// 		inline Coordinate(int32_t line, int32_t column) : line(line), column(column) {}
// 		static Coordinate Invalid()
// 		{
// 			static Coordinate invalid(-1, -1);
// 			return invalid;
// 		}

// 		inline bool operator==(const Coordinate &o) const
// 		{
// 			return line == o.line &&
// 				   column == o.column;
// 		}

// 		inline bool operator!=(const Coordinate &o) const
// 		{
// 			return line != o.line ||
// 				   column != o.column;
// 		}

// 		inline bool operator<(const Coordinate &o) const
// 		{
// 			if (line != o.line)
// 				return line < o.line;
// 			return column < o.column;
// 		}

// 		inline bool operator>(const Coordinate &o) const
// 		{
// 			if (line != o.line)
// 				return line > o.line;
// 			return column > o.column;
// 		}

// 		inline bool operator<=(const Coordinate &o) const
// 		{
// 			if (line != o.line)
// 				return line < o.line;
// 			return column <= o.column;
// 		}

// 		inline bool operator>=(const Coordinate &o) const
// 		{
// 			if (line != o.line)
// 				return line > o.line;
// 			return column >= o.column;
// 		}
// 	};

// 	struct Identifier
// 	{
// 		Coordinate location;
// 		std::string declaration;
// 	};

// };
// }