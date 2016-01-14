#include "gallocy/client.h"

#include <unistd.h>
#include <pthread.h>

#include <cstdlib>
#include <ctime>
#include <iostream>

#include "./restclient.h"
#include "gallocy/logging.h"
#include "gallocy/models.h"
#include "gallocy/stringutils.h"
#include "gallocy/threads.h"


/**
 * Start the client thread.
 */
void GallocyClient::start() {
  if (__gallocy_pthread_create(&thread, nullptr, handle_work, reinterpret_cast<void *>(this))) {
    perror("pthread_create");
  }
}


/**
 * Stop the client thread.
 */
void GallocyClient::stop() {
  alive = false;
  if (__gallocy_pthread_join(thread, nullptr)) {
    perror("pthread_join");
  }
}


/**
 * A pthread invocation wrapper for work function.
 */
void *GallocyClient::handle_work(void *arg) {
  GallocyClient *client = reinterpret_cast<GallocyClient *>(arg);
  void *ret = client->work();
  return ret;
}


/**
 * The work finite state machine.
 */
void *GallocyClient::work() {
  LOG_INFO("Starting HTTP client");
  while (alive) {
    switch (state) {
      case JOINING:
        state = state_joining();
        break;
      case IDLE:
        state = state_idle();
        break;
      default:
        LOG_ERROR("Client reached default handler.");
        break;
    }
    sleep(sleep_duration - std::rand() % sleep_duration);
  }
  return nullptr;
}


/**
 * A joining state.
 */
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


/**
 * An idle state.
 */
GallocyClient::State GallocyClient::state_idle() {
  LOG_INFO("Idle...");
  return IDLE;
}
