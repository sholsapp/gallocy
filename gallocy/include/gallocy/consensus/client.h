#ifndef GALLOCY_CLIENT_H_
#define GALLOCY_CLIENT_H_

#include <pthread.h>

#include <vector>

#include "gallocy/config.h"
#include "gallocy/consensus/state.h"
#include "gallocy/worker.h"


class GallocyClient : public ThreadedDaemon {
 public:
  explicit GallocyClient(GallocyConfig &config) :
    config(config) {}
  /**
   * A Raft follower state.
   */
  RaftState state_follower();
  /**
   * A Raft leader state.
   */
  RaftState state_leader();
  /**
   * A Raft candidate state.
   */
  RaftState state_candidate();
  /**
   * The work finite state machine.
   */
  void *work();
 private:
  GallocyConfig &config;
};

#endif  // GALLOCY_CLIENT_H_
