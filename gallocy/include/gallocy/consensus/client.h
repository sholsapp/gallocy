#ifndef GALLOCY_CLIENT_H_
#define GALLOCY_CLIENT_H_

#include <pthread.h>

#include <vector>

#include "gallocy/config.h"
#include "gallocy/worker.h"


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
  /**
   * An idle state.
   */
  State state_idle();
  /**
   * A joining state.
   */
  State state_joining();
  /**
   * A Raft follower state.
   */
  State state_follower();
  /**
   * A Raft leader state.
   */
  State state_leader();
  /**
   * A Raft candidate state.
   */
  State state_candidate();
  /**
   * The work finite state machine.
   */
  void *work();
 private:
  GallocyConfig &config;
  State state;
  uint64_t step_time;
};

#endif  // GALLOCY_CLIENT_H_
