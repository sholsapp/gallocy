#include <cstring>
#include <cstdlib>

#include "gtest/gtest.h"

#include "gallocy/utils/constants.h"


TEST(ConstantsTests, Addresses) {
  // TODO(sholsapp): Is there a more meaningful way to test this function?
  ASSERT_NE(global_main(), nullptr);
  ASSERT_NE(global_end(), nullptr);
}


TEST(ConstantsTests, HeapLocations) {
  ASSERT_EQ(get_heap_location(PURPOSE_DEVELOPMENT_HEAP), nullptr);
  // TODO(sholsapp): Is there a more meaningful way to test this function?
  ASSERT_NE(get_heap_location(PURPOSE_INTERNAL_HEAP), nullptr);
  ASSERT_NE(get_heap_location(PURPOSE_SHARED_HEAP), nullptr);
  ASSERT_NE(get_heap_location(PURPOSE_APPLICATION_HEAP), nullptr);
}
