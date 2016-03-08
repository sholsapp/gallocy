#include <map>
#include <vector>

#include "gallocy/consensus/client.h"
#include "gallocy/consensus/log.h"
#include "gallocy/entrypoint.h"
#include "gallocy/http/client.h"
#include "gallocy/http/request.h"
#include "gallocy/http/response.h"


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

    // TODO(sholsapp): How we handle this is busted and needs to be refactored so
    // that the cv is usable here. This is also blocking, which is probably bad?
    gallocy::vector<gallocy::http::Request> requests;
    gallocy::map<gallocy::string, gallocy::string> headers;
    headers["Content-Type"] = "application/json";
    for (auto &peer : config.peer_list) {
        requests.push_back(gallocy::http::Request("POST", peer, "/raft/request_vote", j.dump(), headers));
    }
    // TODO(sholsapp): How we handle this is busted and needs to be refactored so
    // that the cv is usable here. This is also blocking, which is probably bad?
    uint64_t votes = gallocy::http::CurlClient().multirequest(requests, request_vote_callback, nullptr, nullptr);

    LOG_INFO("Received votes from " << votes << "/" << config.peer_list.size() << " peers");
    return votes >= config.peer_list.size() / 2;
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
            return success;
        }

        // ON success, update peer's next index and match index.
        if (success) {
            gallocy_state->increment_next_index(peer);
        // ON failure, decremenet peer's next index and try again.
        } else {
            gallocy_state->decrement_next_index(peer);
        }

        return success;
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
    for (auto &peer : config.peer_list)
        requests.push_back(gallocy::http::Request("POST", peer, "/raft/append_entries", j.dump(), headers));
    // TODO(sholsapp): How we handle this is busted and needs to be refactored so
    // that the cv is usable here. This is also blocking, which is probably bad?
    uint64_t votes = gallocy::http::CurlClient().multirequest(requests, append_entries_callback, nullptr, nullptr);
    LOG_DEBUG("Received " << votes << " for append entries");

    // APPLY commands to the state machine.
    if (votes) {
        int64_t idx = gallocy_state->get_log()->log.size() - 1;
        if (idx > 0) {
            gallocy_state->set_commit_index(static_cast<uint64_t>(idx));
            gallocy_state->set_last_applied(static_cast<uint64_t>(idx));
        }
    }

    // RESPOND with results.
    return true;
}
