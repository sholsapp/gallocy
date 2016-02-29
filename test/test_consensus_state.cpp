#include <vector>

#include "gtest/gtest.h"

#include "gallocy/allocators/internal.h"
#include "gallocy/consensus/log.h"
#include "gallocy/consensus/state.h"
#include "gallocy/peer.h"
#include "gallocy/utils/config.h"


TEST(ConsensusStateTests, RaftStateToString) {
  ASSERT_EQ(gallocy::consensus::raft_state_to_string(gallocy::consensus::RaftState::FOLLOWER).compare("FOLLOWER"), 0);
  ASSERT_EQ(gallocy::consensus::raft_state_to_string(gallocy::consensus::RaftState::CANDIDATE).compare("CANDIDATE"), 0);
  ASSERT_EQ(gallocy::consensus::raft_state_to_string(gallocy::consensus::RaftState::LEADER).compare("LEADER"), 0);
}


TEST(ConsensusStateTests, DefaultState) {
  gallocy::string address = "127.0.0.1";
  gallocy::vector<gallocy::common::Peer> peer_list;
  GallocyConfig config(address, peer_list, 1234);
  gallocy::consensus::GallocyState state(config);
  ASSERT_EQ(state.get_commit_index(), static_cast<uint64_t>(0));
  ASSERT_EQ(state.get_current_term(), static_cast<uint64_t>(0));
  ASSERT_EQ(state.get_last_applied(), static_cast<uint64_t>(0));
  ASSERT_EQ(state.get_voted_for(), gallocy::common::Peer());
  ASSERT_EQ(state.get_state(), gallocy::consensus::RaftState::FOLLOWER);
  ASSERT_EQ(state.get_next_index(gallocy::common::Peer()), static_cast<uint64_t>(0));
  ASSERT_EQ(state.get_match_index(gallocy::common::Peer()), static_cast<uint64_t>(0));
}


TEST(ConsensusStateTests, SetState) {
  gallocy::string address = "127.0.0.1";
  gallocy::vector<gallocy::common::Peer> peer_list;
  GallocyConfig config(address, peer_list, 1234);
  gallocy::consensus::GallocyState state(config);
  ASSERT_EQ(state.get_state(), gallocy::consensus::RaftState::FOLLOWER);
  ASSERT_EQ(state.get_timer()->get_step(), static_cast<uint64_t>(FOLLOWER_STEP_TIME));
  ASSERT_EQ(state.get_timer()->get_jitter(), static_cast<uint64_t>(FOLLOWER_JITTER_TIME));
  state.set_state(gallocy::consensus::RaftState::CANDIDATE);
  ASSERT_EQ(state.get_timer()->get_step(), static_cast<uint64_t>(FOLLOWER_STEP_TIME));
  ASSERT_EQ(state.get_timer()->get_jitter(), static_cast<uint64_t>(FOLLOWER_JITTER_TIME));
  state.set_state(gallocy::consensus::RaftState::LEADER);
  ASSERT_EQ(state.get_timer()->get_step(), static_cast<uint64_t>(LEADER_STEP_TIME));
  ASSERT_EQ(state.get_timer()->get_jitter(), static_cast<uint64_t>(LEADER_JITTER_TIME));
}


TEST(ConsensusStateTests, TimerConfiguration) {
  ASSERT_GE(FOLLOWER_STEP_TIME, LEADER_STEP_TIME);
  float follower_leader_ratio = (FOLLOWER_STEP_TIME - FOLLOWER_JITTER_TIME) / (LEADER_STEP_TIME - LEADER_JITTER_TIME);
  ASSERT_GE(follower_leader_ratio, 3.0);
}


TEST(ConsensusStateTests, AppendLogEntry) {
  gallocy::string address = "127.0.0.1";
  gallocy::vector<gallocy::common::Peer> peer_list;
  GallocyConfig config(address, peer_list, 1234);
  gallocy::consensus::GallocyState state(config);
  gallocy::consensus::Command command("noop");
  gallocy::consensus::LogEntry entry(command, 0);
  uint64_t index = state.get_log()->append_entry(entry);
  ASSERT_EQ(index, static_cast<uint64_t>(0));
}
