#include "gtest/gtest.h"

#include "gallocy/consensus/state.h"

TEST(ConsensusStateTests, DefaultState) {
  GallocyState state;
  ASSERT_EQ(state.get_commit_index(), 0);
  ASSERT_EQ(state.get_current_term(), 0);
  ASSERT_EQ(state.get_last_applied(), 0);
  ASSERT_EQ(state.get_voted_for(), 0);
  // ASSERT_EQ(state.get_next_index(0), 0);
  // ASSERT_EQ(state.get_match_index(0), 0);
}
