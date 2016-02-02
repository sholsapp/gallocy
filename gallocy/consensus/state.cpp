#include "gallocy/allocators/internal.h"
#include "gallocy/consensus/state.h"


const gallocy::string raft_state_to_string(RaftState state) {
  if (state == RaftState::FOLLOWER)
    return "FOLLOWER";
  else if (state == RaftState::LEADER)
    return "LEADER";
  else if (state == RaftState::CANDIDATE)
    return "CANDIDATE";
  else
    abort();
}
