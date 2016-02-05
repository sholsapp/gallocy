#include "gtest/gtest.h"

#include "gallocy/consensus/log.h"
#include "gallocy/consensus/state.h"


TEST(ConsensusStateTests, RaftStateToString) {
  ASSERT_EQ(raft_state_to_string(RaftState::FOLLOWER).compare("FOLLOWER"), 0);
  ASSERT_EQ(raft_state_to_string(RaftState::CANDIDATE).compare("CANDIDATE"), 0);
  ASSERT_EQ(raft_state_to_string(RaftState::LEADER).compare("LEADER"), 0);
}


TEST(ConsensusStateTests, DefaultState) {
  GallocyState state;
  ASSERT_EQ(state.get_commit_index(), 0);
  ASSERT_EQ(state.get_current_term(), 0);
  ASSERT_EQ(state.get_last_applied(), 0);
  ASSERT_EQ(state.get_voted_for(), 0);
  ASSERT_EQ(state.get_state(), RaftState::FOLLOWER);
  ASSERT_EQ(state.get_next_index(0), 0);
  ASSERT_EQ(state.get_match_index(0), 0);
}


TEST(ConsensusStateTests, SetState) {
  GallocyState state;
  ASSERT_EQ(state.get_state(), RaftState::FOLLOWER);
  ASSERT_EQ(state.get_timer()->get_step(), FOLLOWER_STEP_TIME);
  ASSERT_EQ(state.get_timer()->get_jitter(), FOLLOWER_JITTER_TIME);
  state.set_state(RaftState::CANDIDATE);
  ASSERT_EQ(state.get_timer()->get_step(), FOLLOWER_STEP_TIME);
  ASSERT_EQ(state.get_timer()->get_jitter(), FOLLOWER_JITTER_TIME);
  state.set_state(RaftState::LEADER);
  ASSERT_EQ(state.get_timer()->get_step(), LEADER_STEP_TIME);
  ASSERT_EQ(state.get_timer()->get_jitter(), LEADER_JITTER_TIME);
}


TEST(ConsensusStateTests, AppendLogEntry) {
  GallocyState state;
  Command command("noop");
  LogEntry entry(command, 0);
  int64_t index = state.get_log()->append_entry(entry);
  ASSERT_EQ(index, 0);
}
