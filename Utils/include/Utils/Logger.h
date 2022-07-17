#pragma once

#include "Color.h"

#include <string>
#include <vector>
namespace Utils
{
	class Logger
	{
	public:
		template <class... ArgsType>
		inline static void Debug(std::string_view fmt, ArgsType &&...args)
		{
			std::string msg = PrintWithColor<ArgsType...>("white", fmt, args...);
			buffer.emplace_back(msg, MetaData{MetaData::Debug});
		}

		template <class... ArgsType>
		inline static void Info(std::string_view fmt, ArgsType &&...args)
		{
			std::string msg = PrintWithColor<ArgsType...>("blue", fmt, args...);
			buffer.emplace_back(msg, MetaData{MetaData::Info});
		}

		template <class... ArgsType>
		inline static void Error(std::string_view fmt, ArgsType &&...args)
		{
			std::string msg = PrintWithColor<ArgsType...>("red", fmt, args...);
			buffer.emplace_back(msg, MetaData{MetaData::Error});
		}

		template <class... ArgsType>
		inline static void Success(std::string_view fmt, ArgsType &&...args)
		{
			std::string msg = PrintWithColor<ArgsType...>("green", fmt, args...);
			buffer.emplace_back(msg, MetaData{MetaData::Success});
		}

		template <class... ArgsType>
		inline static void Warning(std::string_view fmt, ArgsType &&...args)
		{
			std::string msg = PrintWithColor<ArgsType...>("yellow", fmt, args...);
			buffer.emplace_back(msg, MetaData{MetaData::Warning});
		}
		struct MetaData
		{
			enum Level
			{
				Debug,
				Error,
				Info,
				Success,
				Warning,
				COUNT
			} level;
		};

		static std::vector<std::pair<std::string, MetaData>> buffer;

	private:
		Logger() = default;
	};

}