#ifndef GALLOCY_CLIENT_H_
#define GALLOCY_CLIENT_H_

#include <pthread.h>

#include <vector>

#include "gallocy/config.h"
#include "gallocy/worker.h"


class HTTPClient {
 public:
  void request();
  void multirequest();
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
