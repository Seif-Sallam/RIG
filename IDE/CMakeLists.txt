cmake_minimum_required(VERSION 3.16)

FILE(GLOB_RECURSE SOURCES src/IDE/*.cpp)
FILE(GLOB_RECURSE HEADERS include/IDE/*.h)

add_library(IDE
	STATIC
	${SOURCES}
	${HEADERS}
)

target_include_directories(IDE
	PRIVATE
	include/
)
target_link_libraries(IDE
	PUBLIC
	GL
	glad
	glfw
	glm
	imgui
)