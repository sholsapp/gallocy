#include <cstring>
#include <cstdlib>

#include "gtest/gtest.h"

#include "gallocy/utils/config.h"


TEST(ConfigTests, LoadConfig) {
  GallocyConfig *config = load_config("test/data/config.json");
  // TODO(sholsapp): Free memory.
  ASSERT_NE(config, nullptr);
}


TEST(ConfigTests, LoadConfigNoPeers) {
  GallocyConfig *config = load_config("test/data/config-no-peers.json");
  ASSERT_EQ(config->peers.size(), 0);
  // TODO(sholsapp): Free memory.
  ASSERT_NE(config, nullptr);
}
