cmake_minimum_required(VERSION 3.16)

project(
	Rig
	VERSION
	0.1
	LANGUAGES
	CXX
)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Parser)
add_subdirectory(Rig)