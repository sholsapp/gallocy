#include "libgallocy.h"

#include <cstring>
#include <cstdlib>
#include <iostream>

#include "gtest/gtest.h"


TEST(JsonTests, BasicTest) {
  gallocy::json j;
  j["string"] = "abc";
  j["int"] = 1;
  j["float"] = 3.141;
  j["bool"] = true;
  j["list"] = { "foo", "bar", "baz" };
  j["object"] = { { "sub-int", 1 } };
  ASSERT_EQ(j["string"], "abc");
  ASSERT_EQ(j["int"], 1);
  ASSERT_EQ(j["float"], 3.141);
  ASSERT_EQ(j["bool"], true);
  ASSERT_EQ(j["list"].size(), 3);
  ASSERT_EQ(j["list"][0], "foo");
  ASSERT_EQ(j["list"][1], "bar");
  ASSERT_EQ(j["list"][2], "baz");
  ASSERT_EQ(j["list"][3], gallocy::json(nullptr));
  ASSERT_EQ(j["object"]["sub-int"], 1);
}

TEST(JsonTests, ParseTest) {
  gallocy::json j = gallocy::json::parse("{\"happy\":true,\"list\":[\"a\",\"b\"],\"pi\":3.141}");
  ASSERT_EQ(j["happy"], true);
  ASSERT_EQ(j["pi"], 3.141);
  ASSERT_EQ(j["list"].size(), 2);
  ASSERT_EQ(j["list"][0], "a");
  ASSERT_EQ(j["list"][1], "b");
}


TEST(JsonTests, DumpTest) {
  gallocy::json j;
  j["happy"] = true;
  j["pi"] = 3.141;
  j["list"] = { "a", "b" };
  ASSERT_EQ(j.dump(), "{\"happy\":true,\"list\":[\"a\",\"b\"],\"pi\":3.141}");
}
