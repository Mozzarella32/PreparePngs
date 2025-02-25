cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(PreparePngs)

add_executable(${CMAKE_PROJECT_NAME} main.cpp)

project(xxd LANGUAGES C CXX)

file(GLOB XXD_SOURCES
    "submodules/xxd/src/*.c"
)

add_executable(xxd ${XXD_SOURCES})

add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:xxd> ${PROJECT_SOURCE_DIR}/
    COMMENT "Copy executable to project dir"
)


add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:PreparePngs> ${PROJECT_SOURCE_DIR}/
    COMMENT "Copy executable to project dir"
)