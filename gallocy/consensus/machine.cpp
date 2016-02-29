#include <unistd.h>
#include <pthread.h>

#include <cstdlib>
#include <ctime>
#include <iostream>

#include "gallocy/consensus/machine.h"
#include "gallocy/entrypoint.h"
#include "gallocy/models.h"
#include "gallocy/peer.h"
#include "gallocy/threads.h"
#include "gallocy/utils/logging.h"
#include "gallocy/utils/stringutils.h"


void *GallocyMachine::work() {
  LOG_DEBUG("Starting HTTP client");

  if (gallocy_state == nullptr) {
    LOG_ERROR("The gallocy_state object is null.");
    abort();
  }

  gallocy_state->get_timer()->start();

  while (alive) {
    std::unique_lock<std::mutex> lk(gallocy_state->get_timer_mutex());
    // Wait here indefinitely until the alarm expires.
    gallocy_state->get_timer_cv().wait(lk);
    // If the timer expires the condition will be signaled, which indicates
    // that our leader failed to contact us in a timely manner. When this
    // happens, transition to a candidate state.
    if (gallocy_state->get_state() == RaftState::FOLLOWER) {
      gallocy_state->set_state(RaftState::CANDIDATE);
    }

    switch (gallocy_state->get_state()) {
      case RaftState::FOLLOWER:
        gallocy_state->set_state(state_candidate());
        break;
      case RaftState::LEADER:
        gallocy_state->set_state(state_leader());
        break;
      case RaftState::CANDIDATE:
        gallocy_state->set_state(state_candidate());
        break;
      default:
        LOG_ERROR("Client reached default handler.");
        break;
    }
  }
  return nullptr;
}


RaftState GallocyMachine::state_follower() {
  return RaftState::FOLLOWER;
}


RaftState GallocyMachine::state_leader() {
  gallocy_client->send_append_entries();
  return RaftState::LEADER;
}


RaftState GallocyMachine::state_candidate() {
  if (gallocy_client->send_request_vote()) {
    gallocy_state->set_state(RaftState::LEADER);
    gallocy_state->get_timer()->reset();
    return state_leader();
  } else {
    gallocy_state->get_timer()->reset();
    gallocy_state->set_voted_for(gallocy::common::Peer());
    return RaftState::CANDIDATE;
  }
}
