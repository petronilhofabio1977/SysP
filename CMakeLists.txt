cmake_minimum_required(VERSION 3.16)
project(SysP)
include_directories(src)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(sy ${SOURCES})
