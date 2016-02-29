#ifndef GALLOCY_CONSENSUS_STATE_H_
#define GALLOCY_CONSENSUS_STATE_H_

#include <condition_variable>
#include <map>
#include <mutex>

#include "gallocy/allocators/internal.h"
#include "gallocy/consensus/log.h"
#include "gallocy/consensus/timer.h"
#include "gallocy/utils/config.h"
#include "gallocy/utils/logging.h"


#define FOLLOWER_STEP_TIME 2000
#define FOLLOWER_JITTER_TIME 500
#define LEADER_STEP_TIME 500
#define LEADER_JITTER_TIME 0


/**
 * The type to identifier peers.
 */
using PeerId = uint64_t;


/**
 * Raft states.
 */
enum RaftState {
  FOLLOWER,
  LEADER,
  CANDIDATE,
};


/**
 * Convert a raft state to a string representation.
 */
const gallocy::string raft_state_to_string(RaftState state);


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
  explicit GallocyState(GallocyConfig &config) :
      current_term(0),
      voted_for(0),
      commit_index(0),
      last_applied(0),
      config(config) {
    timer = new (internal_malloc(sizeof(Timer)))
      Timer(FOLLOWER_STEP_TIME, FOLLOWER_JITTER_TIME, std::addressof(timed_out));
    log = new (internal_malloc(sizeof(GallocyLog)))
      GallocyLog();
    state = RaftState::FOLLOWER;
  }
  ~GallocyState() {
    timer->~Timer();
    internal_free(timer);
    log->~GallocyLog();
    internal_free(log);
  }
  GallocyState(const GallocyState &) = delete;
  GallocyState &operator=(const GallocyState &) = delete;
  /**
   * Get the current term.
   */
  uint64_t get_current_term() {
    std::lock_guard<std::mutex> lock(access_lock);
    return current_term;
  }
  /**
   * Set the current term.
   *
   * In setting a new current term, the state that tracks the candidate we
   * last voted for is implicitly reset.
   */
  void set_current_term(uint64_t value) {
    std::lock_guard<std::mutex> lock(access_lock);
    if (value < current_term)
      LOG_ERROR("Regressing current term from "
          << current_term
          << " to "
          << value
          << ". This is a logic error.");
    current_term = value;
    voted_for = 0;
  }
  /**
   * Get candidate that received vote in current term.
   */
  PeerId get_voted_for() {
    std::lock_guard<std::mutex> lock(access_lock);
    return voted_for;
  }
  /**
   * Set candidate that received vote in current term.
   */
  void set_voted_for(PeerId value) {
    std::lock_guard<std::mutex> lock(access_lock);
    voted_for = value;
  }
  /**
   * Get the commit index.
   */
  uint64_t get_commit_index() {
    std::lock_guard<std::mutex> lock(access_lock);
    return commit_index;
  }
  /**
   * Set the commit index.
   */
  void set_commit_index(uint64_t value) {
    std::lock_guard<std::mutex> lock(access_lock);
    commit_index = value;
  }
  /**
   * Get the index of the last applied log entry.
   */
  uint64_t get_last_applied() {
    std::lock_guard<std::mutex> lock(access_lock);
    return last_applied;
  }
  /**
   * Set the index of the last applied log entry.
   */
  void set_last_applied(uint64_t value) {
    std::lock_guard<std::mutex> lock(access_lock);
    last_applied = value;
  }
  /**
   * Get index of the next log entry to send to peer.
   */
  uint64_t get_next_index(PeerId peer) {
    std::lock_guard<std::mutex> lock(access_lock);
    return next_index[peer];
  }
  /**
   * Set index of the next log entry to send to peer.
   */
  void set_next_index(PeerId peer, uint64_t value) {
    std::lock_guard<std::mutex> lock(access_lock);
    next_index[peer] = value;
  }
  /**
   * Get index of highest log entry known to be replicated on peer.
   */
  uint64_t get_match_index(PeerId peer) {
    std::lock_guard<std::mutex> lock(access_lock);
    return match_index[peer];
  }
  /**
   * Set index of highest log entry known to be replicated on peer.
   */
  void set_match_index(PeerId peer, uint64_t value) {
    std::lock_guard<std::mutex> lock(access_lock);
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
  GallocyLog *log;

  //
  // Volatile state on all servers.
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
  GallocyConfig &config;
  RaftState state;
  /**
   * A lock to synchronize all get/set access to private member data.
   */
  std::mutex access_lock;
  /**
   * The timer.
   *
   * This timer is used to implement the Raft consensus module and repeatedly
   * counts down from a randomized starting time. If the count down ever
   * reaches zero, which indicates a leader timeout, the `timed_out` condition
   * variable is signaled.
   */
  Timer *timer;
  std::condition_variable timed_out;
  std::mutex timed_out_mutex;

 public:
  /**
   * Get the state machine log.
   */
  GallocyLog *get_log() {
    return log;
  }
  /**
   * Get the timer.
   */
  Timer *get_timer() {
    return timer;
  }
  /**
   * Get the timer's mutex.
   */
  std::mutex &get_timer_mutex() {
    return timed_out_mutex;
  }
  /**
   * Get the timer condition variable.
   *
   * To get this timer's mutex, use :meth:`get_timer_mutex`.
   */
  std::condition_variable &get_timer_cv() {
    return timed_out;
  }
  /**
   * Get the current state of this node.
   */
  RaftState get_state() {
    std::lock_guard<std::mutex> lock(access_lock);
    return state;
  }
  /**
   * Set the current state of this node.
   */
  void set_state(RaftState new_state) {
    std::lock_guard<std::mutex> lock(access_lock);

    // Only log state changes.
    if (new_state != state)
      LOG_INFO("Changing state from "
          << L_ORANGE(raft_state_to_string(state))
          << " to "
          << L_ORANGE(raft_state_to_string(new_state))
          << " in "
          << "("
          << "term=" << current_term << ", "
          << "l.a.=" << last_applied << ", "
          << "c.i.=" << commit_index
          << ")");

    if (new_state == RaftState::LEADER) {
      timer->set_step(LEADER_STEP_TIME);
      timer->set_jitter(LEADER_JITTER_TIME);
      // TODO(sholsapp): Initialize the leader here for each peer.
    } else {
      timer->set_step(FOLLOWER_STEP_TIME);
      timer->set_jitter(FOLLOWER_JITTER_TIME);
    }
    state = new_state;
  }
};

#endif  // GALLOCY_CONSENSUS_STATE_H_
