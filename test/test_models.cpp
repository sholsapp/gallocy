#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gtest/gtest.h"

#include "gallocy/libgallocy.h"
#include "gallocy/models.h"


TEST(ModelsTests, PeerInfo) {

  Engine e;

  e.create_table(PeerInfo::CREATE_STATEMENT);
  for (int i = 0; i < 16; ++i)
    e.execute(PeerInfo(i, i, 0, false).insert());
}
