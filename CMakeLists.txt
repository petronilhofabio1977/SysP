cmake_minimum_required(VERSION 3.16)

project(SysP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect all source files
file(GLOB_RECURSE SOURCES
    src/*.cpp
)

add_executable(sysp ${SOURCES})

target_include_directories(sysp PRIVATE src)

# Useful warnings
target_compile_options(sysp PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
