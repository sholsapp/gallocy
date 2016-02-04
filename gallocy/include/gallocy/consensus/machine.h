#ifndef GALLOCY_CONSENSUS_MACHINE_H_
#define GALLOCY_CONSENSUS_MACHINE_H_

#include <pthread.h>

#include <vector>

#include "gallocy/consensus/state.h"
#include "gallocy/utils/config.h"
#include "gallocy/worker.h"


/**
 * The gallocy finite state machine.
 *
 * The gallocy finite state machine, referred to simply as the gallocy machine,
 * models the various states of the Raft consensus protocol and implements an
 * asynchronous daemon that transitions the state machine between states.
 */
class GallocyMachine : public ThreadedDaemon {
 public:
  explicit GallocyMachine(GallocyConfig &config) :
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

#endif  // GALLOCY_CONSENSUS_MACHINE_H_
