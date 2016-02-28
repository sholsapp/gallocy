if(CMAKE_COMPILER_IS_GNUCXX)
	# include(CodeCoverage)
	# message("Using CodeCoverage module")
	# setup_target_for_coverage(${PROJECT_NAME}_coverage gallocy_tests coverage)
endif()


set (GALLOCY_INCLUDE
  ${CURL_INCLUDE_DIRS}
  ../gallocy
  ../gallocy/include
  ../gallocy/include/gallocy
  ../gallocy/external/restclient-cpp
  ../gallocy/external/sqlite3)
include_directories(${GALLOCY_INCLUDE})
include_directories(${GTEST_INCLUDE_DIRS})

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

set(CMAKE_CXX_FLAGS "-std=c++0x -g -Wall -fstack-protector-all -Wstack-protector -fno-omit-frame-pointer ${CMAKE_CXX_FLAGS_COVERAGE}")

set(test_sources
  gtest.cpp
  test_config.cpp
  test_consensus_server.cpp
  test_consensus_state.cpp
  test_consensus_timer.cpp
  test_constants.cpp
  test_transport.cpp
  test_diff.cpp
  test_free.cpp
  test_httpd.cpp
  test_json.cpp
  test_logging.cpp
  test_malloc.cpp
  test_mmult.cpp
  test_models.cpp
  test_singleton.cpp
  test_stlallocator.cpp
  test_stringutils.cpp
  test_threads.cpp
)

add_executable(gallocy_tests ${test_sources})
target_link_libraries(gallocy_tests gtest_main gallocy-core gallocy-runtime pthread "dl" ${CURL_LIBRARIES})
install(TARGETS gallocy_tests DESTINATION bin)
