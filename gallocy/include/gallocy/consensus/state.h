#ifndef GALLOCY_CONSENSUS_STATE_H_
#define GALLOCY_CONSENSUS_STATE_H_

#include <condition_variable>
#include <map>
#include <mutex>
#include <vector>

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
 * This abstraction focuses on maintaining internal Raft state of the node.
 * Both client and server abstractions interact with this object to change
 * state. This abstraction does not focus on receiving, transmitting, encoding,
 * or decoding Raft messages, unless it makes sense to do so for cleanliness.
 * For protocol details, see \ref gallocy::consensus::GallocyClient and \ref
 * gallocy::consensus::GallocyServer.
 *
 * Conceptually, this abstraction captures most of the logic present in the
 * Raft consensus protocol. For example, this abstraction implements the logic
 * that determines if a vote can be granted of if a log entry can be
 * replicated. Additionally, this abstraction transparently implements
 * internal state such as the timeout counter.
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
    // CREATE a timer
    timer = new (internal_malloc(sizeof(gallocy::consensus::Timer)))
      gallocy::consensus::Timer(FOLLOWER_STEP_TIME, FOLLOWER_JITTER_TIME, std::addressof(timed_out));
    // CREATE a log
    log = new (internal_malloc(sizeof(gallocy::consensus::GallocyLog)))
      gallocy::consensus::GallocyLog();
    // SET initial state
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
  void set_voted_for(const gallocy::common::Peer &value);
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
  /**
   * Increment current term.
   *
   * \return The current term, after incrementing.
   */
  uint64_t increment_current_term();
  /**
   * Increment next index for peer.
   *
   * \param peer The peer's whose metadata to adjust.
   * \return The peer's next index, after incrementing.
   */
  uint64_t increment_next_index(const gallocy::common::Peer &peer);
  /**
   * Decrement next index for peer.
   *
   * \param peer The peer's whose metadata to adjust.
   * \return The peer's next index, after decrementing.
   */
  uint64_t decrement_next_index(const gallocy::common::Peer &peer);
  /**
   * Increment match index for peer.
   *
   * \param peer The peer's whose metadata to adjust.
   * \return The peer's match index, after incrementing.
   */
  uint64_t increment_match_index(const gallocy::common::Peer &peer);

  /**
   * Evaluate a request for a vote.
   *
   * Upon a vote being granted to the peer, this function returns true and sets
   * internal state metadata to reflect the vote.
   *
   * See \ref gallocy::consensus::GallocyServer for usage.
   *
   * \param peer The peer requesting a vote.
   * \param candidate_commit_index The peer's commit index.
   * \param candidate_current_term The peer's current term.
   * \param candidate_last_applied The peer's last applied command.
   * \return True if the vote has been granted.
   */
  bool try_grant_vote(const gallocy::common::Peer &peer,
                      uint64_t candidate_commit_index,
                      uint64_t candidate_current_term,
                      uint64_t candidate_last_applied);

  /**
   * Evaluate a request to replicate log entries.
   *
   * See \ref gallocy::consensus::GallocyServer for usage.
   *
   * \param leader_entries The log entries to try to replicate.
   * \param leader_commit_index The leader's commit index.
   * \param leader_prev_log_index The leader's previous log index.
   * \param leader_prev_log_term The leader's previous log term.
   * \param leader_term The leader's term.
   * \return True if the log entries were successfully replicated.
   */
  bool try_replicate_log(const gallocy::common::Peer &peer,
                         const gallocy::vector<gallocy::consensus::LogEntry> &leader_entries,
                         uint64_t leader_commit_index,
                         uint64_t leader_prev_log_index,
                         uint64_t leader_prev_log_term,
                         uint64_t leader_term);

  /**
   * Check and apply committed but unapplied log entries.
   *
   * This method checks internal state for committed but unapplied log entries
   * and applies them to the local state machine. This method is suitable for
   * use in all servers: leaders, candidates, and followers, and is safe to
   * call at any time.
   *
   * \return True if a log entry was applied.
   */
  bool try_apply();

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
