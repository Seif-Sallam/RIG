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
	../Utils/include
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_link_directories(IDE
		PUBLIC
		GL
	)
endif()

target_link_libraries(IDE
	PUBLIC
	glad
	glfw
	glm
	imgui
	Utils
)