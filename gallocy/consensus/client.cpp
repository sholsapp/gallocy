#include <unistd.h>
#include <pthread.h>

#include <cstdlib>
#include <ctime>
#include <iostream>

#include "./restclient.h"
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
  return LEADER;
}


GallocyClient::State GallocyClient::state_candidate() {
  utils::multirequest("/raft/request_vote", config.peers, config.port);
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
