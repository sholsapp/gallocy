#include <map>
#include <vector>

#include "gallocy/consensus/client.h"
#include "gallocy/consensus/log.h"
#include "gallocy/entrypoint.h"
#include "gallocy/http/client.h"
#include "gallocy/http/request.h"
#include "gallocy/http/response.h"


std::function<bool(const Response &)> request_vote_callback = [](const Response &rsp) {
  if (rsp.status_code == 200) {
    gallocy::string body = rsp.body.c_str();
    gallocy::json response_json = gallocy::json::parse(body.c_str());
    bool granted = response_json["vote_granted"];
    uint64_t supporter_term = response_json["term"];
    uint64_t local_term = gallocy_state->get_current_term();
    if (supporter_term > local_term) {
      gallocy_state->set_state(RaftState::FOLLOWER);
      gallocy_state->set_current_term(supporter_term);
    }
    return granted;
  }
  return false;
};


bool GallocyClient::send_request_vote() {
  uint64_t candidate_term = gallocy_state->get_current_term();
  uint64_t candidate_last_applied = gallocy_state->get_last_applied();
  uint64_t candidate_commit_index = gallocy_state->get_commit_index();

  gallocy::json j = {
    { "term", candidate_term },
    { "last_applied", candidate_last_applied },
    { "commit_index", candidate_commit_index },
  };

  // TODO(sholsapp): How we handle this is busted and needs to be refactored so
  // that the cv is usable here. This is also blocking, which is probably bad?
  gallocy::vector<Request> requests;
  gallocy::map<gallocy::string, gallocy::string> headers;
  headers["Content-Type"] = "application/json";
  for (auto &peer : config.peer_list) {
    requests.push_back(Request("POST", peer, "/raft/request_vote", j.dump(), headers));
  }
  // TODO(sholsapp): How we handle this is busted and needs to be refactored so
  // that the cv is usable here. This is also blocking, which is probably bad?
  uint64_t votes = CurlClient().multirequest(requests, request_vote_callback, nullptr, nullptr);

  LOG_INFO("Received votes from " << votes << "/" << config.peer_list.size() << " peers");
  return votes >= config.peer_list.size() / 2;
}


std::function<bool(const Response &)> append_entries_callback = [](const Response &rsp) {
  if (rsp.status_code == 200) {
    gallocy::string body = rsp.body.c_str();
    gallocy::json response_json = gallocy::json::parse(body.c_str());
    bool success = response_json["success"];
    uint64_t supporter_term = response_json["term"];
    uint64_t local_term = gallocy_state->get_current_term();
    if (supporter_term > local_term) {
      gallocy_state->set_state(RaftState::FOLLOWER);
      gallocy_state->set_current_term(supporter_term);
    }
    return success;
  }
  return false;
};


bool GallocyClient::send_append_entries() {
  uint64_t leader_term = gallocy_state->get_current_term();
  // uint64_t leader_last_applied = gallocy_state->get_last_applied();
  uint64_t leader_commit_index = gallocy_state->get_commit_index();
  uint64_t leader_prev_log_index = gallocy_state->get_log()->get_previous_log_index();
  uint64_t leader_prev_log_term = gallocy_state->get_log()->get_previous_log_term();

  gallocy::json j = {
    // A heartbeat request is an "append entries" with no entries.
    { "entries", gallocy::json::array() },
    { "leader_commit", leader_commit_index },
    { "previous_log_index", leader_prev_log_index },
    { "previous_log_term", leader_prev_log_term },
    { "term", leader_term },
  };

  // TODO(sholsapp): How we handle this is busted and needs to be refactored so
  // that the cv is usable here. This is also blocking, which is probably bad?
  gallocy::vector<Request> requests;
  gallocy::map<gallocy::string, gallocy::string> headers;
  headers["Content-Type"] = "application/json";
  for (auto &peer : config.peer_list)
    requests.push_back(Request("POST", peer, "/raft/append_entries", j.dump(), headers));
  // TODO(sholsapp): How we handle this is busted and needs to be refactored so
  // that the cv is usable here. This is also blocking, which is probably bad?
  uint64_t votes = CurlClient().multirequest(requests, append_entries_callback, nullptr, nullptr);
  // TODO(sholsapp): What is the correct value to return true here?
  return votes > 0;
}


bool GallocyClient::send_append_entries(const gallocy::vector<LogEntry> &entries) {
  uint64_t leader_term = gallocy_state->get_current_term();
  // uint64_t leader_last_applied = gallocy_state->get_last_applied();
  uint64_t leader_commit_index = gallocy_state->get_commit_index();
  uint64_t leader_prev_log_index = gallocy_state->get_log()->get_previous_log_index();
  uint64_t leader_prev_log_term = gallocy_state->get_log()->get_previous_log_term();

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

  gallocy::vector<Request> requests;
  gallocy::map<gallocy::string, gallocy::string> headers;
  headers["Content-Type"] = "application/json";
  for (auto &peer : config.peer_list)
    requests.push_back(Request("POST", peer, "/raft/append_entries", j.dump(), headers));
  // TODO(sholsapp): How we handle this is busted and needs to be refactored so
  // that the cv is usable here. This is also blocking, which is probably bad?
  uint64_t votes = CurlClient().multirequest(requests, append_entries_callback, nullptr, nullptr);
  // TODO(sholsapp): What is the correct value to return true here?
  return votes > 0;
}
