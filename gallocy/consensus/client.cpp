#include <unistd.h>
#include <pthread.h>

#include <cstdlib>
#include <ctime>
#include <iostream>

#include "restclient.h"
#include "gallocy/consensus/client.h"
#include "gallocy/entrypoint.h"
#include "gallocy/logging.h"
#include "gallocy/models.h"
#include "gallocy/stringutils.h"
#include "gallocy/threads.h"
#include "gallocy/utils/http.h"


void *GallocyClient::work() {
  LOG_INFO("Starting HTTP client");

  if (gallocy_state == nullptr) {
    LOG_ERROR("The gallocy_state object is null.");
    abort();
  }

  gallocy_state->start_timer();

  while (alive) {

    std::unique_lock<std::mutex> lk(gallocy_state->get_timer_mutex());
    // Wait here indefinitely until the alarm expires.
    gallocy_state->get_timer_cv().wait(lk);

    switch (state) {
      case FOLLOWER:
        LOG_DEBUG("I'm a follower.");
        state = state_candidate();
        break;
      case LEADER:
        LOG_DEBUG("I'm a leader.");
        state = state_leader();
        break;
      case CANDIDATE:
        LOG_DEBUG("I'm a candidate.");
        state = state_candidate();
        break;
      default:
        LOG_ERROR("Client reached default handler.");
        break;
    }
    // TODO(sholsapp): What to do with this... delete it?
    // sleep(step_time - std::rand() % step_time);
  }
  return nullptr;
}


GallocyClient::State GallocyClient::state_follower() {
  return FOLLOWER;
}


GallocyClient::State GallocyClient::state_leader() {
  LOG_ERROR("Leader state!");

  uint64_t leader_term = gallocy_state->get_current_term();
  uint64_t leader_last_applied = gallocy_state->get_last_applied();
  uint64_t leader_commit_index = gallocy_state->get_commit_index();

  std::function<bool(const RestClient::response &)> callback = [](const RestClient::response &rsp) {
    if (rsp.code == 200) {
      LOG_INFO("Response " << rsp.body.c_str());
      gallocy::string body = rsp.body.c_str();
      gallocy::json response_json = gallocy::json::parse(body);
      return static_cast<bool>(response_json["success"]);
    }
    return false;
  };

  gallocy::json j = {
    { "current_term", leader_term },
  };

  uint64_t votes = utils::post_many("/raft/append_entries", config.peers, config.port, j.dump(), callback);
  LOG_ERROR("Received responses from " << votes << "/" << config.peers.size() << " peers");
  return LEADER;
}


GallocyClient::State GallocyClient::state_candidate() {
  // Increment our term.
  gallocy_state->set_current_term(gallocy_state->get_current_term() + 1);

  uint64_t candidate_term = gallocy_state->get_current_term();
  uint64_t candidate_last_applied = gallocy_state->get_last_applied();
  uint64_t candidate_commit_index = gallocy_state->get_commit_index();

  std::function<bool(const RestClient::response &)> callback = [](const RestClient::response &rsp) {
    if (rsp.code == 200) {
      gallocy::string body = rsp.body.c_str();
      gallocy::json response_json = gallocy::json::parse(body);
      return static_cast<bool>(response_json["vote_granted"]);
    }
    return false;
  };

  gallocy::json j = {
    { "current_term", candidate_term },
    { "last_applied", candidate_last_applied },
    { "commit_index", candidate_commit_index },
  };

  uint64_t votes = utils::post_many("/raft/request_vote", config.peers, config.port, j.dump(), callback);
  LOG_ERROR("Received votes from " << votes << "/" << config.peers.size() << " peers");
  if (votes > config.peers.size() / 2) {
    return state_leader();
  }
  return CANDIDATE;
}


#if 0
GallocyClient::State GallocyClient::state_joining() {
  // TODO(sholsapp): don't bug peers that we're already connected to by cross
  // references the peer_info_table.
  for (auto peer : config.peers) {
    gallocy::stringstream url;
    gallocy::json json_body;
    // TODO(sholsapp): This isn't implicitly converting to a
    // gallocy::json::string_t? Without this c_str(), the JSON payload turns
    // into an array of characters.
    json_body["ip_address"] = config.address.c_str();
    json_body["is_master"] = config.master;
    url << "http://" << peer << ":" << config.port << "/join";

    RestClient::response rsp = RestClient::post(
        url.str().c_str(),
        "application/json",
        json_body.dump());

    if (rsp.code == 200) {
      gallocy::string body = rsp.body.c_str();
      body = utils::trim(body);
      LOG_INFO(url.str()
        << " - "
        << rsp.code
        << " - "
        << body);
    } else {
      LOG_INFO(url.str()
        << " - "
        << rsp.code);
    }
  }

  if (peer_info_table.all().size() != config.peers.size()) {
    return JOINING;
  } else {
    return IDLE;
  }
}

#endif
