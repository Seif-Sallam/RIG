#pragma once
#include <string>
#include <list>

struct Token
{
	enum Type : uint16_t
	{
		COMMA,
		COLON,
		HASHTAG,
		LABEL,
		INST,
		L_PARAN,
		R_PARAN,
		NUMBER,
		REGISTER
	} type = INST;
	std::string data;
	Token *next;
	void Prepare();
};