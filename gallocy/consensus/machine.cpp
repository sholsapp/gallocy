#include <unistd.h>
#include <pthread.h>

#include <cstdlib>
#include <ctime>
#include <iostream>

#include "gallocy/common/peer.h"
#include "gallocy/consensus/machine.h"
#include "gallocy/entrypoint.h"
#include "gallocy/models.h"
#include "gallocy/threads.h"
#include "gallocy/utils/logging.h"
#include "gallocy/utils/stringutils.h"


void *gallocy::consensus::GallocyMachine::work() {
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
    // If the timer expires, our leader failed to contact us in a timely
    // manner, which indicates we need to start an election.
    if (gallocy_state->get_state() == gallocy::consensus::RaftState::FOLLOWER) {
      gallocy_state->set_state(gallocy::consensus::RaftState::CANDIDATE);
    }

    switch (gallocy_state->get_state()) {
      case gallocy::consensus::RaftState::FOLLOWER:
        gallocy_state->set_state(state_candidate());
        break;
      case gallocy::consensus::RaftState::LEADER:
        gallocy_state->set_state(state_leader());
        break;
      case gallocy::consensus::RaftState::CANDIDATE:
        gallocy_state->set_state(state_candidate());
        break;
      default:
        LOG_ERROR("Client reached default handler.");
        break;
    }
  }
  return nullptr;
}


gallocy::consensus::RaftState gallocy::consensus::GallocyMachine::state_follower() {
  return gallocy::consensus::RaftState::FOLLOWER;
}


gallocy::consensus::RaftState gallocy::consensus::GallocyMachine::state_leader() {
  gallocy_client->send_append_entries();
  return gallocy::consensus::RaftState::LEADER;
}


gallocy::consensus::RaftState gallocy::consensus::GallocyMachine::state_candidate() {
  if (gallocy_client->send_request_vote()) {
    gallocy_state->set_state(gallocy::consensus::RaftState::LEADER);
    gallocy_state->get_timer()->reset();
    return state_leader();
  } else {
    gallocy_state->get_timer()->reset();
    gallocy_state->set_voted_for(gallocy::common::Peer());
    return gallocy::consensus::RaftState::CANDIDATE;
  }
}
