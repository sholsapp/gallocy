#include "gtest/gtest.h"


TEST(SanityTest, AssertionTrue) {
    ASSERT_EQ(1, 1) << "Sanity check failed.";
}

#if 0
TEST(SanityTest, Death) {
  int *i = NULL;
  ASSERT_DEATH({ *((int *) i) = 0; }, "");
}
#endif
