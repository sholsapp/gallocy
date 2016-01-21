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
  };
 public:
  explicit GallocyClient(GallocyConfig &config) :
    config(config),
    state(JOINING),
    sleep_duration(5) {}
  State state_idle();
  State state_joining();
  void *work();
 private:
  GallocyConfig &config;
  State state;
  uint64_t sleep_duration;
};

#endif  // GALLOCY_CLIENT_H_
