cmake_minimum_required(VERSION 3.16)

set(SOURCES
	./src/Rig/main.cpp
	./src/Rig/Parser.cpp)

set(HEADERS
	./include/Rig/Parser.h)

add_executable(
	RIG
	${SOURCES}
	${HEADERS}
)

target_include_directories(
	RIG
	PRIVATE
	./include/
)

target_link_libraries(RIG
	PRIVATE
	Parser
)