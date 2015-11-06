#include <cstring>
#include <cstdlib>

#include "gtest/gtest.h"

#include "config.h"
#include "stringutils.h"


TEST(ConfigTests, ReadFileTest) {
  gallocy::string contents(utils::read_file("test/data/config.json"));
  ASSERT_EQ(contents, "{\"foo\": \"bar\"}\n");
}


TEST(ConfigTests, LoadConfig) {
  gallocy::json config = load_config("test/data/config.json");
  ASSERT_EQ(config["foo"], "bar");
}
