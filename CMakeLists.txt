cmake_minimum_required(VERSION 2.6)

project(gallocy)

set(CMAKE_INSTALL_PREFIX install)

set(LIBRARY_OUTPUT_PATH build/lib)
set(EXECUTABLE_OUTPUT_PATH build/bin)

add_subdirectory(gtest-1.7.0)
add_subdirectory(gallocy)
add_subdirectory(test)
