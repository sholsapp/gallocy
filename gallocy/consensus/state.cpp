#include <stdexcept>

#include "gallocy/allocators/internal.h"
#include "gallocy/common/peer.h"
#include "gallocy/consensus/log.h"
#include "gallocy/consensus/state.h"
#include "gallocy/consensus/timer.h"


const gallocy::string gallocy::consensus::raft_state_to_string(gallocy::consensus::RaftState state) {
    if (state == gallocy::consensus::RaftState::FOLLOWER)
        return "FOLLOWER";
    else if (state == gallocy::consensus::RaftState::LEADER)
        return "LEADER";
    else if (state == gallocy::consensus::RaftState::CANDIDATE)
        return "CANDIDATE";
    else
        abort();
}


uint64_t gallocy::consensus::GallocyState::get_current_term() const {
    std::lock_guard<std::mutex> lock(std::ref(access_lock));
    return current_term;
}


void gallocy::consensus::GallocyState::set_current_term(uint64_t value) {
    std::lock_guard<std::mutex> lock(access_lock);
    if (value < current_term)
        LOG_ERROR("Regressing current term from "
                << current_term << " to " << value
                << ". This is a logic error.");
    current_term = value;
    voted_for = gallocy::common::Peer();
}


gallocy::common::Peer gallocy::consensus::GallocyState::get_voted_for() const {
    std::lock_guard<std::mutex> lock(access_lock);
    return voted_for;
}


void gallocy::consensus::GallocyState::set_voted_for(const gallocy::common::Peer &value) {
    std::lock_guard<std::mutex> lock(access_lock);
    voted_for = value;
}


uint64_t gallocy::consensus::GallocyState::get_commit_index() const {
    std::lock_guard<std::mutex> lock(access_lock);
    return commit_index;
}


void gallocy::consensus::GallocyState::set_commit_index(uint64_t value) {
    std::lock_guard<std::mutex> lock(access_lock);
    commit_index = value;
}


uint64_t gallocy::consensus::GallocyState::get_last_applied() const {
    std::lock_guard<std::mutex> lock(access_lock);
    return last_applied;
}


void gallocy::consensus::GallocyState::set_last_applied(uint64_t value) {
    std::lock_guard<std::mutex> lock(access_lock);
    last_applied = value;
}


uint64_t gallocy::consensus::GallocyState::get_next_index(const gallocy::common::Peer &peer) const {
    std::lock_guard<std::mutex> lock(access_lock);
    try {
        return next_index.at(peer);
    } catch (const std::out_of_range& oor) {
        return 0;
    }
}


void gallocy::consensus::GallocyState::set_next_index(const gallocy::common::Peer &peer, uint64_t value) {
    std::lock_guard<std::mutex> lock(access_lock);
    next_index[peer] = value;
}


uint64_t gallocy::consensus::GallocyState::get_match_index(const gallocy::common::Peer &peer) const {
    std::lock_guard<std::mutex> lock(access_lock);
    try {
        return match_index.at(peer);
    } catch (const std::out_of_range& oor) {
        return 0;
    }
}


void gallocy::consensus::GallocyState::set_match_index(const gallocy::common::Peer &peer, uint64_t value) {
    std::lock_guard<std::mutex> lock(access_lock);
    match_index[peer] = value;
}

gallocy::consensus::GallocyLog *gallocy::consensus::GallocyState::get_log()  const {
    return log;
}


gallocy::consensus::Timer *gallocy::consensus::GallocyState::get_timer() const {
    return timer;
}


std::mutex &gallocy::consensus::GallocyState::get_timer_mutex() {
    return timed_out_mutex;
}


std::condition_variable &gallocy::consensus::GallocyState::get_timer_cv() {
    return timed_out;
}


gallocy::consensus::RaftState gallocy::consensus::GallocyState::get_state() const {
    std::lock_guard<std::mutex> lock(access_lock);
    return state;
}


void gallocy::consensus::GallocyState::initialize_leader_state() {
    for (auto peer : config.peer_list) {
        next_index[peer] = commit_index + 1;
        match_index[peer] = 0;
    }
}


void gallocy::consensus::GallocyState::finalize_leader_state() {
    next_index.clear();
    match_index.clear();
}


void gallocy::consensus::GallocyState::set_state(gallocy::consensus::RaftState new_state) {
    std::lock_guard<std::mutex> lock(access_lock);

    // Only log state changes.
    if (new_state != state)
        LOG_INFO("Changing state from "
                << L_ORANGE(gallocy::consensus::raft_state_to_string(state))
                << " to "
                << L_ORANGE(gallocy::consensus::raft_state_to_string(new_state))
                << " in "
                << "("
                << "term=" << current_term << ", "
                << "l.a.=" << last_applied << ", "
                << "c.i.=" << commit_index
                << ")");

    if (new_state == gallocy::consensus::RaftState::LEADER) {
        // We're the leader now
        initialize_leader_state();
        timer->set_step(LEADER_STEP_TIME);
        timer->set_jitter(LEADER_JITTER_TIME);
    } else {
        // We're a candidate or follower now
        finalize_leader_state();
        timer->set_step(FOLLOWER_STEP_TIME);
        timer->set_jitter(FOLLOWER_JITTER_TIME);
    }
    state = new_state;
}


gallocy::json gallocy::consensus::GallocyState::to_json() const {
    gallocy::json j = {
        { "term", get_current_term() },
        { "state", raft_state_to_string(get_state()).c_str() },
        { "commit_index", get_commit_index() },
        { "last_applied", get_last_applied() },
        { "voted_for", get_voted_for().get_string().c_str() },
        { "log_size", get_log()->log.size() },
    };
    return j;
}


uint64_t gallocy::consensus::GallocyState::increment_current_term() {
    std::lock_guard<std::mutex> lock(access_lock);
    current_term += 1;
    return current_term;
}


uint64_t gallocy::consensus::GallocyState::increment_next_index(const gallocy::common::Peer &peer) {
    std::lock_guard<std::mutex> lock(access_lock);
    next_index[peer] += 1;
    return next_index[peer];
}


uint64_t gallocy::consensus::GallocyState::decrement_next_index(const gallocy::common::Peer &peer) {
    std::lock_guard<std::mutex> lock(access_lock);
    next_index[peer] -= 1;
    return next_index[peer];
}


uint64_t gallocy::consensus::GallocyState::increment_match_index(const gallocy::common::Peer &peer) {
    std::lock_guard<std::mutex> lock(access_lock);
    match_index[peer] += 1;
    return match_index[peer];
}


bool gallocy::consensus::GallocyState::try_grant_vote(
        const gallocy::common::Peer &candidate_voted_for,
        uint64_t candidate_commit_index,
        uint64_t candidate_current_term,
        uint64_t candidate_last_applied) {
    gallocy::common::Peer local_voted_for = get_voted_for();
    uint64_t local_commit_index = get_commit_index();
    uint64_t local_current_term = get_current_term();
    uint64_t local_last_applied = get_last_applied();

    if (candidate_current_term < local_current_term) {
        return false;
    } else if (local_voted_for == gallocy::common::Peer()
            || local_voted_for == candidate_voted_for) {
        if (candidate_last_applied >= local_last_applied
                && candidate_commit_index >= local_commit_index) {
            LOG_INFO("Granting vote to "
                    << candidate_voted_for.get_string()
                    << " in term " << candidate_current_term);

            set_current_term(candidate_current_term);
            set_voted_for(candidate_voted_for);
            get_timer()->reset();
            return true;
        } else {
            // TODO(sholsapp): Implement logic to reject candidates that don't have
            // an up to date log.
            LOG_ERROR("Handling of out-of-date log is unimplemented");
            return false;
        }
    }
    LOG_WARNING("Strange grant vote case met. This may be a logic error.");
    return false;
}


bool try_replicate_log(const gallocy::vector<gallocy::consensus::LogEntry> &leader_entries,
        uint64_t leader_commit_index,
        uint64_t leader_prev_log_index,
        uint64_t leader_prev_log_term,
        uint64_t leader_term) {
    return false;
}
