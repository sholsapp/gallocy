#include <cstring>
#include <cstdlib>

#include "gtest/gtest.h"

#include "gallocy/config.h"
#include "gallocy/stringutils.h"


TEST(ConfigTests, LoadConfig) {
  GallocyConfig config = load_config("test/data/config.json");
}
