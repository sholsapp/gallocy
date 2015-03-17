# The directories that must be included for build
set (GALLOCY_INCLUDE
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


# XXX: Wrap the following build blocks into a gallocy-net module that we can
# move around at will.

# The tinyhttp build
add_executable(httpd httpd.cpp)
target_link_libraries(httpd pthread)
install(TARGETS httpd DESTINATION lib)

# The restclient-cpp build
add_library(restclient SHARED external/restclient-cpp/restclient.cpp)
target_link_libraries(restclient gallocy-core curl)
install(TARGETS restclient DESTINATION lib)

add_executable(httpc rest.cpp)
target_link_libraries(httpc restclient json)
install(TARGETS httpc DESTINATION lib)

# The frozen json build
add_library(json SHARED external/frozen/frozen.cpp)
target_link_libraries(json gallocy-core)
install(TARGETS json DESTINATION lib)
