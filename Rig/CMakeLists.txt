cmake_minimum_required(VERSION 3.16)

set(SOURCES
	./src/Rig/main.cpp)

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
	../RISC/include/
)

target_link_libraries(RIG
	PRIVATE
	RISC
	Parser
)

target_compile_definitions(RIG
	PRIVATE
	RESOURCES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources"
)