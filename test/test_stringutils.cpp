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


TEST(StringutilsTests, Split) {
  gallocy::vector<gallocy::string> v;
  gallocy::string t = "a b c d";
  utils::split(t, ' ', v);
  ASSERT_EQ(v.size(), static_cast<size_t>(4));
}


TEST(StringutilsTests, Trim) {
  gallocy::vector<gallocy::string> v;
  gallocy::string t = "a b c d";
  utils::split(t, ' ', v);
  ASSERT_EQ(v.size(), static_cast<size_t>(4));
  ASSERT_EQ(utils::trim(v[0]), "a");
  ASSERT_EQ(utils::trim(v[1]), "b");
  ASSERT_EQ(utils::trim(v[2]), "c");
  ASSERT_EQ(utils::trim(v[3]), "d");
}


TEST(StringutilsTests, ParseInternetAddress) {
  ASSERT_EQ(utils::parse_internet_address(""), 0);
  ASSERT_EQ(utils::parse_internet_address("127.0.0.1"), 2130706433);
}
