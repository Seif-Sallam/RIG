cmake_minimum_required(VERSION 3.16)

set(SOURCES
    ./src/Assembler/main.cpp)

add_executable(
    Assembler
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(
    Assembler
    PRIVATE
    ./include/
    ../Parser/include/
    ../RISC/include/
)

target_link_libraries(Assembler
    PRIVATE
    RISC
    Parser
)

target_compile_definitions(Assembler
    PRIVATE
    RESOURCES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources/"
)