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
	../Parser/include/
)

target_link_libraries(RIG
	PRIVATE
	Parser
)

target_compile_definitions(RIG
	PRIVATE
	RESOURCES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources"
)