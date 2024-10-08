# Dynamic SDK path
# find_path(WINDOWS_SDK_INCLUDE_DIR NAMES mmdeviceapi.h PATHS "C:/Program Files (x86)/Windows Kits/10/Include" REQUIRED)
# include_directories(${WINDOWS_SDK_INCLUDE_DIR})


cmake_minimum_required(VERSION 3.15)

project(WindowsAudioApp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the library for windows_audio
add_library(windows_audio
            lib/WindowsAudioEnumerator.cpp
            lib/WindowsAudioDevice.cpp
            lib/WindowsAudioFormat.cpp
            lib/com_init.cpp)

# Add Windows SDK include paths
include_directories(C:/Program Files (x86)/Windows Kits/10/Include/10.0.22000.0/um)

# Specify include directories for the library
target_include_directories(windows_audio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Link Windows libraries to windows_audio
target_link_libraries(windows_audio PUBLIC mmdevapi ole32 propsys uuid)

# Add the executable
add_executable(WindowsAudioApp main.cpp)

# Link necessary Windows libraries using the keyword syntax
target_link_libraries(WindowsAudioApp PRIVATE windows_audio)
