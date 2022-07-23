#pragma once
#include <string>
#include <algorithm>
#include <imgui.h>

namespace Util
{
	inline static std::string RemovePrefix(const std::string& prefix, const std::string& str)
	{
		std::string copy = str;
		size_t index = copy.find(prefix);
		if(index == std::string::npos)
			return "\0";
		copy.erase(copy.begin() + index, copy.begin() + prefix.size() + index);
		return copy;
	}

	void BeginGroupPanel(const char* name, const ImVec2& size = ImVec2(0.0f, 0.0f));
	void EndGroupPanel();
}