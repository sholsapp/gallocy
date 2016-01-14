#include <cstring>
#include <cstdlib>

#include "gtest/gtest.h"

#include "gallocy/config.h"
#include "gallocy/stringutils.h"


TEST(ConfigTests, LoadConfig) {
  GallocyConfig *config = load_config("test/data/config.json");
  // TODO(sholsapp): Free memory.
}


TEST(ConfigTests, LoadConfigNoPeers) {
  GallocyConfig *config = load_config("test/data/config-no-peers.json");
  ASSERT_EQ(config->peers.size(), 0);
  // TODO(sholsapp): Free memory.
}
