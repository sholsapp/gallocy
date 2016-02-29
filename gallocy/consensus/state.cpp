#include "gallocy/allocators/internal.h"
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


uint64_t gallocy::consensus::GallocyState::get_current_term() {
    std::lock_guard<std::mutex> lock(access_lock);
    return current_term;
}


void gallocy::consensus::GallocyState::set_current_term(uint64_t value) {
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


gallocy::consensus::PeerId gallocy::consensus::GallocyState::get_voted_for() {
    std::lock_guard<std::mutex> lock(access_lock);
    return voted_for;
}


void gallocy::consensus::GallocyState::set_voted_for(gallocy::consensus::PeerId value) {
    std::lock_guard<std::mutex> lock(access_lock);
    voted_for = value;
}


uint64_t gallocy::consensus::GallocyState::get_commit_index() {
    std::lock_guard<std::mutex> lock(access_lock);
    return commit_index;
}


void gallocy::consensus::GallocyState::set_commit_index(uint64_t value) {
    std::lock_guard<std::mutex> lock(access_lock);
    commit_index = value;
}


uint64_t gallocy::consensus::GallocyState::get_last_applied() {
    std::lock_guard<std::mutex> lock(access_lock);
    return last_applied;
}


void gallocy::consensus::GallocyState::set_last_applied(uint64_t value) {
    std::lock_guard<std::mutex> lock(access_lock);
    last_applied = value;
}


uint64_t gallocy::consensus::GallocyState::get_next_index(gallocy::consensus::PeerId peer) {
    std::lock_guard<std::mutex> lock(access_lock);
    return next_index[peer];
}


void gallocy::consensus::GallocyState::set_next_index(gallocy::consensus::PeerId peer, uint64_t value) {
    std::lock_guard<std::mutex> lock(access_lock);
    next_index[peer] = value;
}


uint64_t gallocy::consensus::GallocyState::get_match_index(gallocy::consensus::PeerId peer) {
    std::lock_guard<std::mutex> lock(access_lock);
    return match_index[peer];
}


void gallocy::consensus::GallocyState::set_match_index(gallocy::consensus::PeerId peer, uint64_t value) {
    std::lock_guard<std::mutex> lock(access_lock);
    match_index[peer] = value;
}

gallocy::consensus::GallocyLog *gallocy::consensus::GallocyState::get_log() {
    return log;
}


gallocy::consensus::Timer *gallocy::consensus::GallocyState::get_timer() {
    return timer;
}


std::mutex &gallocy::consensus::GallocyState::get_timer_mutex() {
    return timed_out_mutex;
}


std::condition_variable &gallocy::consensus::GallocyState::get_timer_cv() {
    return timed_out;
}


gallocy::consensus::RaftState gallocy::consensus::GallocyState::get_state() {
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
