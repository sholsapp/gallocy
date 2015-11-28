# The directories that must be included for build
set(GALLOCY_INCLUDE
  .
  gallocy
  heaplayers
  external/restclient-cpp
  external/sqlite3)
include_directories(${GALLOCY_INCLUDE})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -undefined dynamic_lookup")
  endif()


# Custom compile flags for memory allocator
set(CMAKE_CXX_FLAGS
  "-std=c++0x -g -Wall -fstack-protector-all -Wstack-protector -fno-omit-frame-pointer")

# The gallocy-core build
set(GALLOCY_CORE_SRC
  config.cpp
  diff.cpp
  libgallocy.cpp
  pagetable.cpp
  stringutils.cpp
  external/sqlite3/sqlite3.c)
add_library(gallocy-core SHARED ${GALLOCY_CORE_SRC})
target_link_libraries(gallocy-core gallocy-runtime)
install(TARGETS gallocy-core DESTINATION lib)

# The gallocy-wrapper build
set (GALLOCY_WRAPPER_SOURCE
  wrapper.cpp)
add_library(gallocy-wrapper SHARED ${GALLOCY_WRAPPER_SOURCE})
target_link_libraries(gallocy-wrapper gallocy-core)
install(TARGETS gallocy-wrapper DESTINATION lib)

# The gallocy-net build
set(GALLOCY_NET_SRC
  client.cpp
  server.cpp
  request.cpp
  external/restclient-cpp/restclient.cpp)
add_library(gallocy-net SHARED ${GALLOCY_NET_SRC})
target_link_libraries(gallocy-net gallocy-core curl)
install(TARGETS gallocy-net DESTINATION lib)

set(GALLOCY_RUNTIME
  constants.cpp)
add_library(gallocy-runtime SHARED ${GALLOCY_RUNTIME})
install(TARGETS gallocy-runtime DESTINATION lib)

# The tinyhttpd server and a test client
add_executable(httpd bin/server.cpp)
target_link_libraries(httpd pthread gallocy-net gallocy-core gallocy-runtime)
install(TARGETS httpd DESTINATION bin)
add_executable(httpc bin/rest.cpp)
target_link_libraries(httpc gallocy-net gallocy-core gallocy-runtime)
install(TARGETS httpc DESTINATION bin)

# XXX: refactor this into gallocy-threading or something. Or add it to
# gallocy-core.
add_library(threads SHARED threads.cpp)
target_link_libraries(threads pthread)
install(TARGETS threads DESTINATION lib)

add_executable(stacks bin/pthread.cpp)
target_link_libraries(stacks pthread dl gallocy-core threads)
install(TARGETS stacks DESTINATION bin)
