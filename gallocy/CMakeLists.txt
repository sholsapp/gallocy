set(GALLOCY_INCLUDE
  .
  include
  include/gallocy
  heaplayers
  external/restclient-cpp
  external/sqlite3
  ${CURL_INCLUDE_DIRS})
include_directories(${GALLOCY_INCLUDE})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -undefined dynamic_lookup")
endif()


if(GENERATE_COVERAGE)
  message(STATUS "Enabling gcov support")
  if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(COVERAGE_FLAG "--coverage")
  endif()
  set(CMAKE_CXX_FLAGS_COVERAGE
    "-g -O0 ${COVERAGE_FLAG} -fprofile-arcs -ftest-coverage")
  #set(CMAKE_C_FLAGS_COVERAGE
  #  "-g -O0 ${COVERAGE_FLAG} -fprofile-arcs -ftest-coverage")
  #set(CMAKE_EXE_LINKER_FLAGS_COVERAGE
  #  "")
  #set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE
  #  "")
endif()

# Custom compile flags for memory allocator
set(CMAKE_CXX_FLAGS "-std=c++0x -g -Wall -fstack-protector-all -Wstack-protector -fno-omit-frame-pointer ${CMAKE_CXX_FLAGS_COVERAGE}")

set(GALLOCY_CORE_SRC
  allocators/internal.cpp
  allocators/shared.cpp
  consensus/client.cpp
  consensus/machine.cpp
  consensus/server.cpp
  consensus/state.cpp
  entrypoint.cpp
  http/client.cpp
  http/request.cpp
  http/response.cpp
  libgallocy.cpp
  models.cpp
  sqlite.cpp
  threads.cpp
  utils/config.cpp
  utils/diff.cpp
  utils/http.cpp
  utils/logging.cpp
  utils/objutils.cpp
  utils/stringutils.cpp
  # TODO(sholsapp): See https://github.com/mrtazz/restclient-cpp/issues/41 to
  # consider building restclient-cpp propertly.
  external/restclient-cpp/connection.cc
  external/restclient-cpp/helpers.cc
  external/restclient-cpp/restclient.cc
  external/sqlite3/sqlite3.c)
add_library(gallocy-core SHARED ${GALLOCY_CORE_SRC})
target_link_libraries(gallocy-core gallocy-runtime ${CURL_LIBRARIES} pthread dl)
install(TARGETS gallocy-core DESTINATION lib)

set (GALLOCY_WRAPPER_SOURCE
  wrapper.cpp)
add_library(gallocy-wrapper SHARED ${GALLOCY_WRAPPER_SOURCE})
install(TARGETS gallocy-wrapper DESTINATION lib)

set(GALLOCY_RUNTIME
  utils/constants.cpp)
add_library(gallocy-runtime SHARED ${GALLOCY_RUNTIME})
install(TARGETS gallocy-runtime DESTINATION lib)

# A sample application
add_executable(server bin/server.cpp)
target_link_libraries(server ${CURL_LIBRARIES} gallocy-core gallocy-runtime)
install(TARGETS server DESTINATION bin)

# The sample application that uses the *explicit* api, i.e., it does not
# redefine memory allocator and gallocy memory must be explicitly asked for.
add_executable(application-explicit bin/server.cpp)
target_link_libraries(application-explicit ${CURL_LIBRARIES} gallocy-core gallocy-runtime)
install(TARGETS application-explicit DESTINATION bin)

# The sample application that uses the *implicit* api, i.e., it does redefine
# memory allocator and gallocy memory is implicitly used.
add_executable(application-implicit bin/server.cpp)
target_link_libraries(application-implicit ${CURL_LIBRARIES} gallocy-core gallocy-runtime gallocy-wrapper)
install(TARGETS application-implicit DESTINATION bin)

add_executable(sizes bin/sizes.cpp)
install(TARGETS sizes DESTINATION bin)
