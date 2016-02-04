#include "gallocy/consensus/client.h"
#include "gallocy/entrypoint.h"
#include "gallocy/utils/http.h"
#include "restclient.h"  // NOLINT


bool GallocyClient::send_request_vote() {
  uint64_t candidate_term = gallocy_state->get_current_term();
  uint64_t candidate_last_applied = gallocy_state->get_last_applied();
  uint64_t candidate_commit_index = gallocy_state->get_commit_index();
  std::function<bool(const RestClient::Response &)> callback = [](const RestClient::Response &rsp) {
    if (rsp.code == 200) {
      gallocy::string body = rsp.body.c_str();
      gallocy::json response_json = gallocy::json::parse(body);
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

  gallocy::json j = {
    { "term", candidate_term },
    { "last_applied", candidate_last_applied },
    { "commit_index", candidate_commit_index },
  };

  // TODO(sholsapp): How we handle this is busted and needs to be refactored so
  // that the cv is usable here. This is also blocking, which is probably bad?
  uint64_t votes = utils::post_many("/raft/request_vote", config.peers, config.port, j.dump(), callback);
  LOG_INFO("Received votes from " << votes << "/" << config.peers.size() << " peers");
  return votes >= config.peers.size() / 2;
}
