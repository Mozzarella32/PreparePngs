cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(PreparePngs)

add_executable(${CMAKE_PROJECT_NAME} main.cpp)

find_program(XXD_EXECUTABLE NAMES xxd xxd.ex)

if(NOT XXD_EXECUTABLE)
    message(STATUS "xxd not found, fetching and building xxd...")

    include(FetchContent)

    FetchContent_Declare(
        xxd
        GIT_REPOSITORY https://github.com/ckormanyos/xxd.git
        GIT_TAG main
    )

    FetchContent_MakeAvailable(xxd)

    #set_target_properties(xxd PROPERTIES
    #    POST_BUILD_COMMAND ""
    #)

    #set_target_properties(xxd PROPERTIES
    #    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}
    #)

else()
    message(STATUS "xxd found at ${XXD_EXECUTABLE}")
endif()


#add_custom_command(
#    TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${PROJECT_SOURCE_DIR}/
#    COMMENT "Copy executable to project dir"
#)