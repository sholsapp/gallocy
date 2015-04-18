set (GALLOCY_INCLUDE
  ../gallocy
  ../gallocy/heaplayers
  ../gallocy/external/frozen
  ../gallocy/external/sqlite3)
include_directories(${GALLOCY_INCLUDE})
include_directories(${gtest_SOURCE_DIR}/include)

set(test_sources
  gtest.cpp
  test_config.cpp
  test_diff.cpp
  test_free.cpp
  #test_json.cpp
  test_malloc.cpp
  test_mmult.cpp
  test_pagetable.cpp
  test_singleton.cpp
  test_stlallocator.cpp
  test_threads.cpp
  #test_hashmap.cpp
  #test_freesllist.cpp
)
add_executable(gallocy_tests ${test_sources})
target_link_libraries(gallocy_tests gtest_main gallocy-core gallocy-net threads "dl")

install(TARGETS gallocy_tests DESTINATION bin)
