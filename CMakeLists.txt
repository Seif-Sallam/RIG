cmake_minimum_required(VERSION 3.16)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(CPM)

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)

# GLAD
CPMAddPackage(
	NAME glad
	GITHUB_REPOSITORY Dav1dde/glad
	VERSION 0.1.33
)

# GLM
CPMAddPackage(
	NAME glm
	GITHUB_REPOSITORY g-truc/glm
	GIT_TAG 0.9.9.7
)

# GLFW
CPMAddPackage(
	NAME glfw
	GITHUB_REPOSITORY glfw/glfw
	GIT_TAG 3.3.2
	OPTIONS
	"GLFW_BUILD_TESTS Off"
	"GLFW_BUILD_EXAMPLES Off"
	"GLFW_BUILD_DOCS Off"
	"GLFW_INSTALL Off"
	"GLFW_USE_HYBRID_HPG On"
)

project(
	Rig
	VERSION
	0.1
	LANGUAGES
	CXX
)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

if ( MSVC )
	set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W0")
endif()

add_subdirectory(Thirdparty/imgui)
include_directories(AFTER Thirdparty/imgui)
add_subdirectory(IDE)
add_subdirectory(RISC)
add_subdirectory(Parser)
add_subdirectory(Assembler)
add_subdirectory(Disassembler)
add_subdirectory(Rig)
