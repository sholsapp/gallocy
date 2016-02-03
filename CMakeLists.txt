cmake_minimum_required(VERSION 2.6)

project(gallocy)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

set(CMAKE_INSTALL_PREFIX install)

set(LIBRARY_OUTPUT_PATH build/lib)
set(EXECUTABLE_OUTPUT_PATH build/bin)

add_subdirectory(opt)
add_subdirectory(gallocy)
add_subdirectory(test)
