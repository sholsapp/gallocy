include_directories(${gtest_SOURCE_DIR}/include)
set(test_sources
  gtest.cc
)
add_executable(sample_tests ${test_sources})
target_link_libraries(sample_tests gtest_main)

install(TARGETS sample_tests DESTINATION bin)
