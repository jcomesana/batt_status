cmake_minimum_required (VERSION 3.8.0)
get_filename_component(DIR_NAME ${CMAKE_SOURCE_DIR} NAME)
project(${DIR_NAME} C)
enable_testing()
set (CMAKE_C_STANDARD 99)

# Compilation flags
if ("${CMAKE_C_COMPILER_ID}" MATCHES "GNU")
    set (CMAKE_C_FLAGS "-Wall -Wextra -Werror")
    set (CMAKE_C_FLAGS_RELEASE "-O3")
    set (CMAKE_C_FLAGS_DEBUG "-g -O0 -DDEBUG")
endif()

set (CMAKE_EXPORT_COMPILE_COMMANDS 1)

file(GLOB_RECURSE SOURCES "src/*.c" "src/*.h")
add_executable(${PROJECT_NAME} ${SOURCES})
