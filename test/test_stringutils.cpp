#include "gtest/gtest.h"

#include "gallocy/stringutils.h"


TEST(StringutilsTests, StartsWith) {
  ASSERT_TRUE(utils::startswith("xxxaaabbbccc", "xxx"));
  ASSERT_FALSE(utils::startswith("xxxaaabbbccc", "aaa"));
}


TEST(StringutilsTests, EndsWith) {
  ASSERT_TRUE(utils::endswith("xxxaaabbbccc", "ccc"));
  ASSERT_FALSE(utils::endswith("xxxaaabbbccc", "bbb"));
}
