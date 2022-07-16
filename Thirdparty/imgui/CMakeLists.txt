cmake_minimum_required(VERSION 3.16)

FILE(GLOB SOURCES ./*.cpp)
FILE(GLOB HEADERS ./*.h)

add_library(
    imgui
    STATIC
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(imgui
    PRIVATE
    GL
    glfw
    glad
    glm
)