cmake_minimum_required(VERSION 3.16)

add_library(Parser
	STATIC
	src/Parser/Parser.cpp
	src/Parser/Translator.cpp
	src/Parser/Instruction.cpp
	src/Parser/Token.cpp
	src/Parser/RegisterFile.cpp
	include/Parser/Parser.h
	include/Parser/Translator.h
	include/Parser/Instruction.h
	include/Parser/Token.h
	include/Parser/RegisterFile.h
)

target_include_directories(Parser
	PRIVATE
	include/
)