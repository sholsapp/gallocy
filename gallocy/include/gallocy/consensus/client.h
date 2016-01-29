#ifndef GALLOCY_CLIENT_H_
#define GALLOCY_CLIENT_H_

#include <pthread.h>

#include <vector>

#include "gallocy/config.h"
#include "gallocy/worker.h"


class GallocyClient : public ThreadedDaemon {
 public:
  enum State {
    FOLLOWER,
    LEADER,
    CANDIDATE,
  };
 public:
  explicit GallocyClient(GallocyConfig &config) :
    config(config),
    state(FOLLOWER),
    step_time(5) {}
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
