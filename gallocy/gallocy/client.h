#ifndef GALLOCY_CLIENT_H_
#define GALLOCY_CLIENT_H_

#include <pthread.h>

#include <vector>

#include "gallocy/config.h"
#include "gallocy/libgallocy.h"


class GallocyClient {
 public:
  enum State {
    JOINING,
    IDLE,
  };
 public:
  explicit GallocyClient(GallocyConfig &config) :
    alive(true),
    config(config),
    state(JOINING) {}
  State state_idle();
  State state_joining();
  static void *handle_work(void *arg);
  void *work();
  void start();
  void stop();
 private:
  bool alive;
  GallocyConfig &config;
  State state;
  pthread_t thread;
};

#endif  // GALLOCY_CLIENT_H_
