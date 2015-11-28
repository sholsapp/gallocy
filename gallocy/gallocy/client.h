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
    state(IDLE), alive(true), peers(config.peers) {}
  void start();
  void *work();
  void stop();
  static void *handle_work(void *arg);
 private:
  State state;
  bool alive;
  gallocy::vector<gallocy::string> peers;
  pthread_t thread;
};

#endif  // GALLOCY_CLIENT_H_
