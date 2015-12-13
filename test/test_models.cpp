#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gtest/gtest.h"

#include "gallocy/libgallocy.h"
#include "gallocy/models.h"


TEST(ModelsTests, PeerInfo) {
  // TODO(sholsapp): Move this into a setup/teardown
  // fixture so that we can test multiple times.
  e.initialize();
  e.execute(PeerInfo::CREATE_STATEMENT);
  for (int i = 0; i < 16; ++i)
    e.execute(PeerInfo(i, 0, 0, false).insert());
  gallocy::vector<PeerInfo> peers = peer_info_table.all();
  ASSERT_EQ(peers.size(), static_cast<size_t>(16));
  for (int i = 0; i < 16; ++i) {
    ASSERT_EQ(peers[i].id, static_cast<uint64_t>(i + 1));
    ASSERT_EQ(peers[i].ip_address, static_cast<uint64_t>(i));
  }
}
