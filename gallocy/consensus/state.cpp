#include "gallocy/allocators/internal.h"
#include "gallocy/consensus/log.h"
#include "gallocy/consensus/state.h"
#include "gallocy/consensus/timer.h"


const gallocy::string raft_state_to_string(RaftState state) {
    if (state == RaftState::FOLLOWER)
        return "FOLLOWER";
    else if (state == RaftState::LEADER)
        return "LEADER";
    else if (state == RaftState::CANDIDATE)
        return "CANDIDATE";
    else
        abort();
}


uint64_t GallocyState::get_current_term() {
    std::lock_guard<std::mutex> lock(access_lock);
    return current_term;
}


void GallocyState::set_current_term(uint64_t value) {
    std::lock_guard<std::mutex> lock(access_lock);
    if (value < current_term)
        LOG_ERROR("Regressing current term from "
                << current_term
                << " to "
                << value
                << ". This is a logic error.");
    current_term = value;
    voted_for = gallocy::common::Peer();
}


PeerId GallocyState::get_voted_for() {
    std::lock_guard<std::mutex> lock(access_lock);
    return voted_for;
}


void GallocyState::set_voted_for(PeerId value) {
    std::lock_guard<std::mutex> lock(access_lock);
    voted_for = value;
}


uint64_t GallocyState::get_commit_index() {
    std::lock_guard<std::mutex> lock(access_lock);
    return commit_index;
}


void GallocyState::set_commit_index(uint64_t value) {
    std::lock_guard<std::mutex> lock(access_lock);
    commit_index = value;
}


uint64_t GallocyState::get_last_applied() {
    std::lock_guard<std::mutex> lock(access_lock);
    return last_applied;
}


void GallocyState::set_last_applied(uint64_t value) {
    std::lock_guard<std::mutex> lock(access_lock);
    last_applied = value;
}


uint64_t GallocyState::get_next_index(PeerId peer) {
    std::lock_guard<std::mutex> lock(access_lock);
    return next_index[peer];
}


void GallocyState::set_next_index(PeerId peer, uint64_t value) {
    std::lock_guard<std::mutex> lock(access_lock);
    next_index[peer] = value;
}


uint64_t GallocyState::get_match_index(PeerId peer) {
    std::lock_guard<std::mutex> lock(access_lock);
    return match_index[peer];
}


void GallocyState::set_match_index(PeerId peer, uint64_t value) {
    std::lock_guard<std::mutex> lock(access_lock);
    match_index[peer] = value;
}

GallocyLog *GallocyState::get_log() {
    return log;
}


Timer *GallocyState::get_timer() {
    return timer;
}


std::mutex &GallocyState::get_timer_mutex() {
    return timed_out_mutex;
}


std::condition_variable &GallocyState::get_timer_cv() {
    return timed_out;
}


RaftState GallocyState::get_state() {
    std::lock_guard<std::mutex> lock(access_lock);
    return state;
}


void GallocyState::initialize_leader_state() {
  for (auto peer : config.peer_list) {
    next_index[peer] = commit_index + 1;
    match_index[peer] = 0;
  }
}


void GallocyState::set_state(RaftState new_state) {
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
        initialize_leader_state();
    } else {
        timer->set_step(FOLLOWER_STEP_TIME);
        timer->set_jitter(FOLLOWER_JITTER_TIME);
    }
    state = new_state;
}
