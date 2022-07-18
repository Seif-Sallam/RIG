cmake_minimum_required(VERSION 3.16)

FILE(GLOB_RECURSE SOURCES ./src/Parser/*.cpp)
FILE(GLOB_RECURSE HEADERS ./include/Parser/*.h)

add_library(Parser
	STATIC
	${SOURCES}
	${HEADERS}
)

target_link_libraries(Parser
	PRIVATE
	RISC
	Utils
)

target_include_directories(Parser
	PRIVATE
	./include/
	./../RISC/include/
	./../Utils/include/
)