#include <string.h>
#include <cstring>
#include <string>
#include <stdarg.h>
#include <stdio.h>
#include <map>

#include <Parser/ErrorMessage.h>

#define MAP_ENTRY(enumName)               \
	{                                     \
		ErrorMessage::enumName, #enumName \
	}

namespace Parser
{
	ErrorMessage::ErrorMessage(Type t, const char *fmt, ...) : type(t)
	{
		msg = new char[MAX_MSG_SIZE];
		memset(msg, 0, MAX_MSG_SIZE);
		if (fmt != nullptr)
		{
			uint32_t argCount = 0;
			for (auto p = fmt; *p != '\0'; p++)
				if (*p == '%')
					argCount++;

			if (argCount >= 1)
			{
				va_list args;
				va_start(args, fmt);
				vsprintf(msg, fmt, args);
				va_end(args);
			}
			else
			{
				strcpy(msg, fmt);
			}
		}
	}
	ErrorMessage::ErrorMessage(const ErrorMessage &e)
	{
		msg = new char[MAX_MSG_SIZE];
		memset(msg, 0, MAX_MSG_SIZE);
		for (uint32_t i = 0; i < MAX_MSG_SIZE; i++)
		{
			msg[i] = e.msg[i];
		}
		type = e.type;
	}

	ErrorMessage::ErrorMessage(ErrorMessage &&e)
	{
		msg = e.msg;
		e.msg = nullptr;
		type = e.type;
	}
	ErrorMessage::operator bool()
	{
		return type != NO_ERROR;
	}
	ErrorMessage &ErrorMessage::operator=(const ErrorMessage &e)
	{
		if (this->msg == e.msg && this->type == e.type)
			return *this;
		msg = new char[MAX_MSG_SIZE];
		memset(msg, 0, MAX_MSG_SIZE);
		for (uint32_t i = 0; i < MAX_MSG_SIZE; i++)
		{
			msg[i] = e.msg[i];
		}
		type = e.type;
		return *this;
	}
	ErrorMessage &ErrorMessage::operator=(ErrorMessage &&e)
	{
		msg = e.msg;
		e.msg = nullptr;
		type = e.type;
		return *this;
	}

	ErrorMessage::~ErrorMessage()
	{
		delete[] msg;
	}

	const std::map<ErrorMessage::Type, const char *> enumNames{
		MAP_ENTRY(INVALID_INSTRUCTION),
		MAP_ENTRY(INVALID_LABEL),
		MAP_ENTRY(INVALID_PARAMETERS),
		MAP_ENTRY(INVALID_SYMBOL),
		MAP_ENTRY(NO_ERROR),
		MAP_ENTRY(INVALID_STRUCTURE_DATA),
		MAP_ENTRY(INVALID_STRUCTURE_TEXT),
		MAP_ENTRY(INVALID_DIRECTIVE),
		MAP_ENTRY(INVALID_DIRECTIVE_DATA),
	};

	std::ostream &operator<<(std::ostream &stream, const ErrorMessage &message)
	{
		stream << "Error: " << enumNames.at(message.type) << "\n\tMessage: " << message.msg << '\n';
		return stream;
	}
} // namespace Parser