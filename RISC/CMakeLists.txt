cmake_minimum_required(VERSION 3.16)

add_library(RISC
    STATIC
    src/RISC/Translator.cpp
    src/RISC/Instruction.cpp
    src/RISC/RegisterFile.cpp
    include/RISC/Translator.h
    include/RISC/Instruction.h
    include/RISC/RegisterFile.h
)

target_include_directories(RISC
    PRIVATE
    include/
)