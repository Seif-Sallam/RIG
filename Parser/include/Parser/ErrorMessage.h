#pragma once

#include <iostream>

#define MAX_MSG_SIZE 1024u

namespace Parser
{
	struct ErrorMessage
	{
		enum Type : uint32_t
		{
			NO_ERROR = 1,
			INVALID_LABEL,
			INVALID_SYMBOL,
			INVALID_INSTRUCTION,
			INVALID_PARAMETERS,
			INVALID_STRUCTURE_DATA,
			INVALID_STRUCTURE_TEXT,
			INVALID_DIRECTIVE,
			INVALID_DIRECTIVE_DATA,
			COUNT
		} type = NO_ERROR;
		char *msg;
		ErrorMessage(Type t = NO_ERROR, const char *fmt = nullptr, ...);
		ErrorMessage(const ErrorMessage &e);
		ErrorMessage(ErrorMessage &&e);
		operator bool();
		ErrorMessage &operator=(const ErrorMessage &e);
		ErrorMessage &operator=(ErrorMessage &&e);
		~ErrorMessage();
	};

	std::ostream &operator<<(std::ostream &stream, const ErrorMessage &message);
}