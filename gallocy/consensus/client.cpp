#include <algorithm>
#include <condition_variable>
#include <map>
#include <mutex>
#include <vector>

#include "gallocy/consensus/client.h"
#include "gallocy/consensus/log.h"
#include "gallocy/entrypoint.h"
#include "gallocy/http/client.h"
#include "gallocy/http/request.h"
#include "gallocy/http/response.h"


bool gallocy::consensus::GallocyClient::raft_request(const gallocy::vector<gallocy::http::Request> &requests,
        std::function<bool(const gallocy::http::Response &)> callback) {
    std::condition_variable have_majority;
    std::mutex count_lock;
    uint64_t peer_count = config.peer_list.size();
    uint64_t peer_majority = peer_count / 2;
    uint64_t rsp_count = 0;
    // SEND requests
    gallocy::http::CurlClient().multirequest(
            requests,
            callback,
            std::ref(rsp_count),
            std::addressof(have_majority),
            std::addressof(count_lock));
    std::unique_lock<std::mutex> lk(count_lock);
    // LOOP while we do not have a majority of responses.
    while (rsp_count < peer_majority) {
        // WAIT for responses
        std::cv_status status = have_majority.wait_for(lk, std::chrono::milliseconds(1));
        // IF timed out waiting, we've not received enough responses.
        if (status == std::cv_status::timeout) {
            // RETURN failure, we've timed out waiting for responses.
            return false;
        }
    }
    // RETURN success, a majority of responses have been received.
    return true;
}


std::function<bool(const gallocy::http::Response &)> request_vote_callback = [](const gallocy::http::Response &rsp) {
    if (rsp.status_code == 200) {
        gallocy::string body = rsp.body.c_str();
        gallocy::json response_json = gallocy::json::parse(body.c_str());
        bool granted = response_json["vote_granted"];
        uint64_t supporter_term = response_json["term"];
        uint64_t local_term = gallocy_state->get_current_term();
        if (supporter_term > local_term) {
            gallocy_state->set_current_term(supporter_term);
            gallocy_state->set_state(gallocy::consensus::RaftState::FOLLOWER);
        }
        return granted;
    }
    return false;
};


bool gallocy::consensus::GallocyClient::send_request_vote() {
    uint64_t candidate_term = gallocy_state->increment_current_term();
    uint64_t candidate_last_applied = gallocy_state->get_last_applied();
    uint64_t candidate_commit_index = gallocy_state->get_commit_index();

    gallocy::json j = {
        { "term", candidate_term },
        { "last_applied", candidate_last_applied },
        { "commit_index", candidate_commit_index },
    };

    gallocy::vector<gallocy::http::Request> requests;
    gallocy::map<gallocy::string, gallocy::string> headers;
    headers["Content-Type"] = "application/json";
    for (auto &peer : config.peer_list) {
        requests.push_back(gallocy::http::Request("POST", peer, "/raft/request_vote", j.dump(), headers));
    }

    return raft_request(requests, request_vote_callback);
}


std::function<bool(const gallocy::http::Response &)> append_entries_callback = [](const gallocy::http::Response &rsp) {
    if (rsp.status_code == 200) {
        gallocy::common::Peer peer = rsp.peer;
        gallocy::string body = rsp.body.c_str();
        gallocy::json response_json = gallocy::json::parse(body.c_str());
        bool success = response_json["success"];
        uint64_t supporter_term = response_json["term"];
        uint64_t local_term = gallocy_state->get_current_term();

        // CONVERT to follower if peer has seen a higher term.
        if (supporter_term > local_term) {
            gallocy_state->set_state(gallocy::consensus::RaftState::FOLLOWER);
            gallocy_state->set_current_term(supporter_term);
            return false;
        }

        // ON success, update peer's next index and match index.
        if (success) {
            gallocy_state->increment_next_index(peer);
            gallocy_state->increment_match_index(peer);
            return true;
        // ON failure, decremenet peer's next index and try again.
        } else {
            gallocy_state->decrement_next_index(peer);
            return false;
        }
    }
    return false;
};


bool gallocy::consensus::GallocyClient::send_append_entries() {
    gallocy::vector<LogEntry> empty;
    return send_append_entries(empty);
}


bool gallocy::consensus::GallocyClient::send_append_entries(const gallocy::vector<LogEntry> &entries) {
    uint64_t leader_term = gallocy_state->get_current_term();
    uint64_t leader_commit_index = gallocy_state->get_commit_index();
    uint64_t leader_prev_log_index = gallocy_state->get_log()->get_previous_log_index();
    uint64_t leader_prev_log_term = gallocy_state->get_log()->get_previous_log_term();

    // APPEND log entries to log
    for (auto &entry : entries) {
        gallocy_state->get_log()->append_entry(entry);
    }

    // BUILD a payload for peers.
    gallocy::json j = {
        { "entries", gallocy::json::array() },
        { "leader_commit", leader_commit_index },
        { "previous_log_index", leader_prev_log_index },
        { "previous_log_term", leader_prev_log_term },
        { "term", leader_term },
    };
    for (auto &entry : entries) {
        j["entries"].push_back(entry.to_json());
    }

    // SEND the payloads for replication
    gallocy::vector<gallocy::http::Request> requests;
    gallocy::map<gallocy::string, gallocy::string> headers;
    headers["Content-Type"] = "application/json";
    for (auto &peer : config.peer_list) {
        requests.push_back(gallocy::http::Request("POST", peer, "/raft/append_entries", j.dump(), headers));
    }

    bool success = raft_request(requests, append_entries_callback);

    // TODO(sholsapp): If there exists an N such that N > commitIndex, a
    // majority of matchIndex[i] >= N, and log[N].term == currentTerm, set
    // commitIndex = N.

    // APPLY commands to the state machine.
    if (success) {
        int64_t idx = gallocy_state->get_log()->log.size() - 1;
        if (idx > 0) {
            gallocy_state->set_commit_index(static_cast<uint64_t>(idx));
        }
        gallocy_state->try_apply();
        return true;
    }
    return false;
}
