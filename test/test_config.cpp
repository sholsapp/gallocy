#include <cstring>
#include <cstdlib>

#include "gtest/gtest.h"

#include "config.h"


TEST(ConfigTests, ReadFileTest) {
  gallocy::string contents(read_file("test/data/config.json"));
  ASSERT_EQ(contents, "{\"foo\": \"bar\"}\n");
}
