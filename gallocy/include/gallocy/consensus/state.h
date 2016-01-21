#ifndef GALLOCY_CONSENSUS_STATE_H_
#define GALLOCY_CONSENSUS_STATE_H_

#include <pthread.h>

#include "gallocy/allocators/internal.h"
#include "gallocy/consensus/log.h"

/**
 * The type to identifier peers.
 */
using PeerId = uint64_t;

class Guard {
 public:
  inline explicit Guard(pthread_mutex_t *lock) {
    _lock = lock;
    pthread_mutex_lock(_lock);
  }
  inline ~Guard() {
    pthread_mutex_unlock(_lock);
  }
 private:
  pthread_mutex_t *_lock;
};

/**
 * State to implement the Raft consensus protocol.
 *
 * All getter and setter methods are synchronized using an internal pthread
 * mutex lock, so additional synchronization is not necessary.
 *
 * See *In Search of an Understandable Consensus Algorithm (Extended
 * Version)* by Diego Ongaro and John Ousterhout.
 */
class GallocyState {
 public:
  GallocyState() :
      current_term(0),
      voted_for(0),
      commit_index(0),
      last_applied(0) {
    lock = PTHREAD_MUTEX_INITIALIZER;
  }
  GallocyState(const GallocyState &) = delete;
  GallocyState &operator=(const GallocyState &) = delete;
  /**
   * Get the current term.
   */
  uint64_t get_current_term() {
    Guard(std::addressof(lock));
    return current_term;
  }
  /**
   * Set the current term.
   */
  void set_current_term(uint64_t value) {
    Guard(std::addressof(lock));
    current_term = value;
  }
  /**
   * Get candidate that received vote in current term.
   */
  PeerId get_voted_for() {
    Guard(std::addressof(lock));
    return voted_for;
  }
  /**
   * Set candidate that received vote in current term.
   */
  void set_voted_for(PeerId value) {
    Guard(std::addressof(lock));
    voted_for = value;
  }
  /**
   * Get the commit index.
   */
  uint64_t get_commit_index() {
    Guard(std::addressof(lock));
    return commit_index;
  }
  /**
   * Set the commit index.
   */
  void set_commit_index(uint64_t value) {
    Guard(std::addressof(lock));
    commit_index = value;
  }
  /**
   * Get the index of the last applied log entry.
   */
  uint64_t get_last_applied() {
    Guard(std::addressof(lock));
    return last_applied;
  }
  /**
   * Set the index of the last applied log entry.
   */
  void set_last_applied(uint64_t value) {
    Guard(std::addressof(lock));
    last_applied = value;
  }
  /**
   * Get index of the next log entry to send to peer.
   */
  uint64_t get_next_index(PeerId peer) {
    Guard(std::addressof(lock));
    return next_index[peer];
  }
  /**
   * Set index of the next log entry to send to peer.
   */
  void set_next_index(PeerId peer, uint64_t value) {
    Guard(std::addressof(lock));
    next_index[peer] = value;
  }
  /**
   * Get index of highest log entry known to be replicated on peer.
   */
  uint64_t get_match_index(PeerId peer) {
    Guard(std::addressof(lock));
    return match_index[peer];
  }
  /**
   * Set index of highest log entry known to be replicated on peer.
   */
  void set_match_index(PeerId peer, uint64_t value) {
    Guard(std::addressof(lock));
    match_index[peer] = value;
  }

 //
 // Persistent state on all servers.
 //
 private:
  /**
   * The latest term this server has seen.
   */
  uint64_t current_term;
  /**
   * The candidate identifier that received vote in current term.
   */
  PeerId voted_for;
  /**
   * The replicated log.
   *
   * Each entry contains a command for the state machine and the term when the
   * entry was received by the leader.
   */
  GallocyLog log;

 //
 //Volatile state on all servers.
 //
 private:
  /**
   * Index of highest log entry known to be committed.
   */
  uint64_t commit_index;
  /**
   * Index of highest log entry applied to state machine.
   */
  uint64_t last_applied;

 //
 // Volatile state on leaders.
 //
 private:
  /**
   * Mapping of peer to next log entry to send to the peer.
   */
  gallocy::map<PeerId, uint64_t> next_index;
  /**
   * Mapping of peer to highest log entry known to be replicated on the peer.
   */
  gallocy::map<PeerId, uint64_t> match_index;

 //
 // Internal implementation details.
 //
 private:
  /**
   * A lock to synchronize all get/set access to private member data.
   */
  pthread_mutex_t lock;
};

#endif  // GALLOCY_CONSENSUS_STATE_H_
