#pragma once

#include "Color.h"

#include <string>
#include <vector>

#include "imgui.h"

namespace Util
{
	class Logger
	{
	public:
		template <class... ArgsType>
		inline static void Debug(std::string_view fmt, ArgsType &&...args)
		{
			if (enabled)
			{
				int oldSize = m_Buf.size();
				int color = 0xFFFFFFFF;
				std::string msg = PrintWithColor<ArgsType...>("[Debug] ", "white", fmt, args...);
				msg.append("\n");
				m_Buf.append(msg.c_str());
				AddOffset(oldSize, color);
			}
		}

		template <class... ArgsType>
		inline static void Info(std::string_view fmt, ArgsType &&...args)
		{
			if (enabled)
			{
				int oldSize = m_Buf.size();
				int color = 0xFFee0000;
				std::string msg = PrintWithColor<ArgsType...>("[Info] ", "blue", fmt, args...);
				msg.append("\n");
				m_Buf.append(msg.c_str());
				AddOffset(oldSize, color);
			}
		}

		template <class... ArgsType>
		inline static void Error(std::string_view fmt, ArgsType &&...args)
		{
			if (enabled)
			{
				int oldSize = m_Buf.size();
				int color = 0xFF0000FF;
				std::string msg = PrintWithColor<ArgsType...>("[Error] ", "red", fmt, args...);
				msg.append("\n");
				m_Buf.append(msg.c_str());
				AddOffset(oldSize, color);
			}
		}

		template <class... ArgsType>
		inline static void Success(std::string_view fmt, ArgsType &&...args)
		{
			if (enabled)
			{
				int oldSize = m_Buf.size();
				int color = 0xFF00FF00;
				std::string msg = PrintWithColor<ArgsType...>("[Success] ", "green", fmt, args...);
				msg.append("\n");
				m_Buf.append(msg.c_str());
				AddOffset(oldSize, color);
			}
		}

		template <class... ArgsType>
		inline static void Warning(std::string_view fmt, ArgsType &&...args)
		{
			if (enabled)
			{
				int oldSize = m_Buf.size();
				int color = 0xFF00FFFF;
				std::string msg = PrintWithColor<ArgsType...>("[Warning] ", "yellow", fmt, args...);
				msg.append("\n");
				m_Buf.append(msg.c_str());
				AddOffset(oldSize, color);
			}
		}

		template <class... ArgsType>
		inline static void Print(std::string_view fmt, ArgsType &&...args)
		{
			if (enabled)
			{
				int oldSize = m_Buf.size();
				int color = 0xFFFFFFFF;
				std::string msg = PrintWithColor<ArgsType...>("[Print] ", "white", fmt, args...);
				msg.append("\n");
				m_Buf.append(msg.c_str());
				AddOffset(oldSize, color);
			}
		}

		static void ClearBuffer();
		static void Draw(std::string_view title, bool *p_open = nullptr, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
		static bool enabled;

	private:
		static void AddOffset(int oldSize, int color);
		Logger() = default;
		static ImGuiTextBuffer m_Buf;
		static ImGuiTextFilter m_Filter;
		static ImVector<int> m_LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
		static ImVector<int> m_Colors;
		static bool m_AutoScroll;
	};
}