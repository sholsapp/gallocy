cmake_minimum_required(VERSION 2.6)

project(gallocy)

set(CMAKE_INSTALL_PREFIX install)

add_subdirectory(gtest-1.7.0)
add_subdirectory(gallocy)
add_subdirectory(test)
