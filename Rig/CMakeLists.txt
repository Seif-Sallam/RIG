cmake_minimum_required(VERSION 3.16)

set(SOURCES
	./src/Rig/main.cpp)

add_executable(
	Rig
	${SOURCES}
	${HEADERS}
)

target_include_directories(
	Rig
	PRIVATE
	./include/
	../Parser/include/
	../RISC/include/
	../IDE/include/
	../Utils/include
)

target_link_libraries(Rig
	PRIVATE
	RISC
	Parser
	IDE
	Utils
)

target_compile_definitions(Rig
	PRIVATE
	RESOURCES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources"
)