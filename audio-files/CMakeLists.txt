cmake_minimum_required(VERSION 3.15)

project(test)

set(CMAKE_EXE_LINKER_FLAGS "-mconsole")
set(CMAKE_CXX_STANDARD 14)

add_executable(test main.cpp)

