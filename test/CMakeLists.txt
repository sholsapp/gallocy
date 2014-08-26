set (GALLOCY_INCLUDE
  ../gallocy
  ../gallocy/heaplayers
  ../gallocy/external/sqlite3)
include_directories(${GALLOCY_INCLUDE})
include_directories(${gtest_SOURCE_DIR}/include)

set(test_sources
  gtest.cpp
  simple-malloc.cpp
  simple-free.cpp
  mmult.cpp
)
add_executable(sample_tests ${test_sources})
target_link_libraries(sample_tests gtest_main gallocy-core "dl")

install(TARGETS sample_tests DESTINATION bin)
