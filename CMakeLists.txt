cmake_minimum_required(VERSION 3.16)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

project(
	Rig
	VERSION
	0.1
	LANGUAGES
	CXX
)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(RISC)
add_subdirectory(Parser)
add_subdirectory(Assembler)
add_subdirectory(Disassembler)
add_subdirectory(Rig)
