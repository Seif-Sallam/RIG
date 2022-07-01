cmake_minimum_required(VERSION 3.16)

add_library(Parser
	STATIC
	src/Parser/Parser.cpp
	include/Parser/Parser.h
)

target_link_libraries(Parser
	PRIVATE
	RISC
)

target_include_directories(Parser
	PRIVATE
	./../RISC/include/
	./include/
)