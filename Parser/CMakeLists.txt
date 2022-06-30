cmake_minimum_required(VERSION 3.16)

add_library(Parser
	STATIC
	src/Parser/Parser.cpp
	include/Parser/Parser.h
)

target_include_directories(Parser
	PRIVATE
	include/
)