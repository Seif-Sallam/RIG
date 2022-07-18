cmake_minimum_required(VERSION 3.16)

set(SOURCES
	./src/Disassembler/main.cpp)

add_executable(
	Disassembler
	${SOURCES}
	${HEADERS}
)

target_include_directories(
	Disassembler
	PRIVATE
	./include/
	../Parser/include/
	../RISC/include/
	../Utils/include
)

target_link_libraries(Disassembler
	PRIVATE
	RISC
	Parser
	Utils
)

target_compile_definitions(Disassembler
	PRIVATE
	RESOURCES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources/"
)