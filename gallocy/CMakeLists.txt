# The directories that must be included for build
set (GALLOCY_INCLUDE
  .
  heaplayers
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

# The tinyhttp build
add_library(httpd SHARED httpd.c)
install(TARGETS httpd DESTINATION lib)
