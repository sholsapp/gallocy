#ifndef GALLOCY_CLIENT_H_
#define GALLOCY_CLIENT_H_

#include <pthread.h>

#include <vector>

#include "gallocy/config.h"
#include "gallocy/worker.h"


class ConsensusClient {
 public:
  /**
   *
   */
  void replicate() {
    // Main - Create pthread_condition_t majority_responded
    // Main - Create gallocy::vector<Request> work
    // Main - Create thread pool to execute work
    //   Pool - exit if no work exists
    //   Pool - make request
    //   Pool - interpret response
    //   Pool - update state
    // Main - Wait for majority_responded signal
    // Main - Respond to client
    // Main - Wait for thread pool
  }
};


class GallocyClient : public ThreadedDaemon {
 public:
  enum State {
    JOINING,
    IDLE,
    // Raft
    FOLLOWER,
    LEADER,
    CANDIDATE,
  };
 public:
  explicit GallocyClient(GallocyConfig &config) :
    config(config),
    state(JOINING),
    step_time(5) {}
  State state_idle();
  State state_joining();
  // Raft
  State state_follower();
  State state_leader();
  State state_candidate();
  void *work();
 private:
  GallocyConfig &config;
  State state;
  uint64_t step_time;
};

#endif  // GALLOCY_CLIENT_H_
