# The directories that must be included for build
set(GALLOCY_INCLUDE
  .
  heaplayers
  external/frozen
  external/restclient-cpp
  external/sqlite3)
include_directories(${GALLOCY_INCLUDE})


# Custom compile flags for memory allocator
set(CMAKE_CXX_FLAGS
  "-g -Wall -fstack-protector-all -Wstack-protector -fno-omit-frame-pointer")


# The gallocy-core build
set(GALLOCY_CORE_SRC
  libgallocy.cpp
  pagetable.cpp
  diff.cpp
  external/sqlite3/sqlite3.c)
add_library(gallocy-core SHARED ${GALLOCY_CORE_SRC})
install(TARGETS gallocy-core DESTINATION lib)


# The gallocy-wrapper build
set (GALLOCY_WRAPPER_SOURCE
  wrapper.cpp)
add_library(gallocy-wrapper SHARED ${GALLOCY_WRAPPER_SOURCE})
target_link_libraries(gallocy-wrapper gallocy-core)
install(TARGETS gallocy-wrapper DESTINATION lib)

# The gallocy-net build
set(GALLOCY_NET_SRC
  httpd.cpp
  external/restclient-cpp/restclient.cpp
  external/frozen/frozen.cpp)
add_library(gallocy-net SHARED ${GALLOCY_NET_SRC})
target_link_libraries(gallocy-net curl)
install(TARGETS gallocy-net DESTINATION lib)

# The tinyhttpd server and a test client
add_executable(httpd bin/httpd.cpp)
target_link_libraries(httpd pthread gallocy-net gallocy-core)
install(TARGETS httpd DESTINATION bin)
add_executable(httpc bin/rest.cpp)
target_link_libraries(httpc gallocy-net gallocy-core)
install(TARGETS httpc DESTINATION bin)

# XXX: refactor this into gallocy-threading or something. Or add it to
# gallocy-core.
add_library(threads SHARED threads.cpp)
target_link_libraries(threads pthread)
install(TARGETS threads DESTINATION lib)

add_executable(stacks bin/pthread.cpp)
target_link_libraries(stacks pthread gallocy-core threads)
install(TARGETS stacks DESTINATION bin)
