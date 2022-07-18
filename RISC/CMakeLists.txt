cmake_minimum_required(VERSION 3.16)

FILE(GLOB_RECURSE SOURCES src/RISC/*.cpp)
FILE(GLOB_RECURSE HEADERS include/RISC/*.h)

add_library(RISC
	STATIC
	${SOURCES}
	${HEADERS}
)

target_include_directories(RISC
	PRIVATE
	include/
	../Utils/include
)

target_link_libraries(RISC
	PRIVATE
		Utils
)