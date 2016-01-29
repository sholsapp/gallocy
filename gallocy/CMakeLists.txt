# The directories that must be included for build
set(GALLOCY_INCLUDE
  .
  include
  # TODO(sholsapp): Remove this and refactor include paths
  include/gallocy
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
  allocators/internal.cpp
  allocators/shared.cpp
  consensus/client.cpp
  consensus/server.cpp
  entrypoint.cpp
  #glibc/time.c
  libgallocy.cpp
  models.cpp
  request.cpp
  sqlite.cpp
  threads.cpp
  utils/config.cpp
  utils/diff.cpp
  utils/http.cpp
  utils/logging.cpp
  utils/objutils.cpp
  utils/stringutils.cpp
  external/restclient-cpp/restclient.cpp
  external/sqlite3/sqlite3.c)
add_library(gallocy-core SHARED ${GALLOCY_CORE_SRC})
target_link_libraries(gallocy-core gallocy-runtime ${CURL_LIBRARIES} pthread dl)
install(TARGETS gallocy-core DESTINATION lib)

# The gallocy-wrapper build
set (GALLOCY_WRAPPER_SOURCE
  wrapper.cpp)
add_library(gallocy-wrapper SHARED ${GALLOCY_WRAPPER_SOURCE})
#target_link_libraries(gallocy-wrapper gallocy-core)
install(TARGETS gallocy-wrapper DESTINATION lib)

set(GALLOCY_RUNTIME
  utils/constants.cpp)
add_library(gallocy-runtime SHARED ${GALLOCY_RUNTIME})
install(TARGETS gallocy-runtime DESTINATION lib)

# A sample application
add_executable(server bin/server.cpp)
# TODO(sholsapp): Sort out the libc/libc++ static linking issue to keep
# standard library usage between internal and application uses seperate before
# continuing down this path.
#target_link_libraries(server gallocy-core gallocy-runtime gallocy-wrapper)
target_link_libraries(server gallocy-core gallocy-runtime)
install(TARGETS server DESTINATION bin)
