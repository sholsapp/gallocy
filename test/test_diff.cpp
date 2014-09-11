#include "gtest/gtest.h"

#include "diff.h"


TEST(DiffTests, DiffTest) {
  diff("GGAATGG", 7, "ATG", 3);
}
