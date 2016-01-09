set (GALLOCY_INCLUDE
  ../gallocy
  ../gallocy/gallocy
  ../gallocy/heaplayers
  ../gallocy/external/frozen
  ../gallocy/external/sqlite3)
include_directories(${GALLOCY_INCLUDE})
include_directories(${GTEST_INCLUDE_DIRS})

# Custom compile flags for memory allocator
set(CMAKE_CXX_FLAGS
  "-std=c++11 -g -Wall -fstack-protector-all -Wstack-protector -fno-omit-frame-pointer")

set(test_sources
  gtest.cpp
  test_config.cpp
  test_diff.cpp
  test_free.cpp
  test_httpd.cpp
  test_json.cpp
  test_malloc.cpp
  test_mmult.cpp
  test_models.cpp
  test_singleton.cpp
  test_stlallocator.cpp
  test_stringutils.cpp
  test_threads.cpp
)
add_executable(gallocy_tests ${test_sources})
target_link_libraries(gallocy_tests gtest_main gallocy-core gallocy-net gallocy-runtime threads "dl")

install(TARGETS gallocy_tests DESTINATION bin)
