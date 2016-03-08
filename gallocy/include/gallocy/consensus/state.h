#ifndef GALLOCY_CONSENSUS_STATE_H_
#define GALLOCY_CONSENSUS_STATE_H_

#include <condition_variable>
#include <map>
#include <mutex>

#include "gallocy/allocators/internal.h"
#include "gallocy/common/peer.h"
#include "gallocy/consensus/log.h"
#include "gallocy/consensus/timer.h"
#include "gallocy/utils/config.h"
#include "gallocy/utils/logging.h"


#define FOLLOWER_STEP_TIME 2000
#define FOLLOWER_JITTER_TIME 500
#define LEADER_STEP_TIME 500
#define LEADER_JITTER_TIME 0

namespace gallocy {

namespace consensus {

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
      voted_for(gallocy::common::Peer()),
      commit_index(0),
      last_applied(0),
      config(config) {
    timer = new (internal_malloc(sizeof(gallocy::consensus::Timer)))
      gallocy::consensus::Timer(FOLLOWER_STEP_TIME, FOLLOWER_JITTER_TIME, std::addressof(timed_out));
    log = new (internal_malloc(sizeof(gallocy::consensus::GallocyLog)))
      gallocy::consensus::GallocyLog();
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
  uint64_t get_current_term() const;
  /**
   * Set the current term.
   *
   * In setting a new current term, the state that tracks the candidate we
   * last voted for is implicitly reset.
   */
  void set_current_term(uint64_t value);
  /**
   * Get candidate that received vote in current term.
   */
  gallocy::common::Peer get_voted_for() const;
  /**
   * Set candidate that received vote in current term.
   */
  void set_voted_for(gallocy::common::Peer value);
  /**
   * Get the commit index.
   */
  uint64_t get_commit_index() const;
  /**
   * Set the commit index.
   */
  void set_commit_index(uint64_t value);
  /**
   * Get the index of the last applied log entry.
   */
  uint64_t get_last_applied() const;
  /**
   * Set the index of the last applied log entry.
   */
  void set_last_applied(uint64_t value);
  /**
   * Get index of the next log entry to send to peer.
   */
  uint64_t get_next_index(const gallocy::common::Peer &peer) const;
  /**
   * Set index of the next log entry to send to peer.
   */
  void set_next_index(const gallocy::common::Peer &peer, uint64_t value);
  /**
   * Get index of highest log entry known to be replicated on peer.
   */
  uint64_t get_match_index(const gallocy::common::Peer &peer) const;
  /**
   * Set index of highest log entry known to be replicated on peer.
   */
  void set_match_index(const gallocy::common::Peer &peer, uint64_t value);
  /**
   * Get the state machine log.
   */
  gallocy::consensus::GallocyLog *get_log() const;
  /**
   * Get the timer.
   */
  gallocy::consensus::Timer *get_timer() const;
  /**
   * Get the timer's mutex.
   */
  std::mutex &get_timer_mutex();
  /**
   * Get the timer condition variable.
   *
   * To get this timer's mutex, use :meth:`get_timer_mutex`.
   */
  std::condition_variable &get_timer_cv();
  /**
   * Get the current state of this node.
   */
  RaftState get_state() const;
  /**
   * Set the current state of this node.
   */
  void set_state(RaftState new_state);
  /**
   * Dump the state object as JSON object.
   */
  gallocy::json to_json() const;

 private:
  /**
   * The latest term this server has seen.
   */
  uint64_t current_term;
  /**
   * The candidate identifier that received vote in current term.
   */
  gallocy::common::Peer voted_for;
  /**
   * The replicated log.
   *
   * Each entry contains a command for the state machine and the term when the
   * entry was received by the leader.
   */
  gallocy::consensus::GallocyLog *log;
  /**
   * Index of highest log entry known to be committed.
   */
  uint64_t commit_index;
  /**
   * Index of highest log entry applied to state machine.
   */
  uint64_t last_applied;
  /**
   * Mapping of peer to next log entry to send to the peer.
   */
  gallocy::map<gallocy::common::Peer, uint64_t> next_index;
  /**
   * Mapping of peer to highest log entry known to be replicated on the peer.
   */
  gallocy::map<gallocy::common::Peer, uint64_t> match_index;
  /**
   * A lock to synchronize all get/set access to private member data.
   */
  mutable std::mutex access_lock;
  /**
   * The Raft timer.
   *
   * If the count down ever reaches zero, which indicates a leader timeout, the
   * `timed_out` condition variable is signaled.
   */
  gallocy::consensus::Timer *timer;
  /**
   * The Raft timer's time out condition variable.
   */
  std::condition_variable timed_out;
  /**
   * The Raft timer's time out condition variable mutex.
   */
  std::mutex timed_out_mutex;
  /**
   * The configuration object.
   */
  GallocyConfig &config;
  /**
   * The current Raft state.
   */
  RaftState state;
  /**
   * Set up leader state.
   */
  void initialize_leader_state();
  /**
   * Tear down leader state.
   */
  void finalize_leader_state();
};

}  // namespace consensus

}  // namespace gallocy

#endif  // GALLOCY_CONSENSUS_STATE_H_
