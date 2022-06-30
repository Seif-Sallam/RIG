#include "Parser/Token.h"
#include "Parser/Instruction.h"
#include <map>

const std::map<std::string, Token::Type> tokenTypes = {
	{":", Token::COLON},
	{",", Token::COMMA},
	{"#", Token::HASHTAG},
	{"(", Token::L_PARAN},
	{")", Token::R_PARAN},
};

inline static bool IsDigits(const std::string &str)
{
	for (auto &s : str)
	{
		if (s < '0' || s > '9')
			return false;
	}
	return true;
}

void Token::Prepare()
{
	auto it = tokenTypes.find(data);
	if (it != tokenTypes.end())
	{
		type = it->second;
		return;
	}
	else
	{
		if (IsDigits(data))
		{
			type = Token::NUMBER;
		}
		else
		{
			std::string copy = data;

			for (auto &c : copy)
				c = toupper(c);

			auto &instNames = Instruction::typeNames;

			if (auto it = std::find(instNames.begin(), instNames.end(), copy); it != instNames.end())
			{
				data = copy;
				type = Token::INST;
			}
			// else
			// {
			// 	type = Token::LABEL;
			// }
		}
	}
}