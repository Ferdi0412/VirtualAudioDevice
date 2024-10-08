cmake_minimum_required(VERSION 3.15)

project(virtual_audio)

add_executable(virtual_audio src/main.cpp)

set(CMAKE_CXX_STANDARD 14)

# Add Windows SDK include paths
include_directories(C:/Program Files (x86)/Windows Kits/10/Include/10.0.22000.0/um) # This needs to become dynamic...

# Link to necessary Windows libraries
target_link_libraries(${PROJECT_NAME} mmdevapi ole32)


