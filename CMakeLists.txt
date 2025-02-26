cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(PreparePngs)

add_executable(${PROJECT_NAME} main.cpp)

project(xxd LANGUAGES C CXX)

file(GLOB XXD_SOURCES
    "submodules/xxd/src/*.c"
)

add_executable(xxd ${XXD_SOURCES})

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:xxd> ${CMAKE_SOURCE_DIR}/resources/art
    COMMENT "Copy xxd to art"
)


add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:PreparePngs> ${CMAKE_SOURCE_DIR}/resources/art
    COMMENT "Copy PreparePngs to art"
)
