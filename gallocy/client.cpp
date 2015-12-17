#include "gallocy/client.h"

#include <unistd.h>
#include <pthread.h>

#include <iostream>

#include "./restclient.h"
#include "gallocy/logging.h"
#include "gallocy/stringutils.h"


/**
 * Start the client thread.
 */
void GallocyClient::start() {
  if (pthread_create(&thread, nullptr, handle_work, reinterpret_cast<void *>(this))) {
    perror("pthread_create");
  }
}


/**
 * Stop the client thread.
 */
void GallocyClient::stop() {
  alive = false;
  if (pthread_join(thread, nullptr)) {
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
  while (alive) {
    switch (state) {
      case JOINING:
        state = state_joining();
        break;
      case IDLE:
        state = state_idle();
        break;
      default:
        std::cout << "Default..." << std::endl;
        break;
    }
    sleep(5);
  }
  return nullptr;
}


GallocyClient::State GallocyClient::state_joining() {
  for (auto peer : config.peers) {
    gallocy::stringstream url;
    gallocy::json json_body;
    json_body["ip_address"] = config.address.c_str();
    json_body["is_master"] = config.master;
    url << "http://" << peer << ":" << config.port << "/join";
    gallocy::string _test = json_body.dump().c_str();

    RestClient::response rsp = RestClient::post(
        url.str().c_str(),
        "application/json",
        _test.c_str());

    if (rsp.code == 200) {
      gallocy::string body = rsp.body.c_str();
      body = utils::trim(body);
      LOG_INFO(url.str()
        << " - "
        << rsp.code
        << " - "
        << body)
    } else {
      LOG_INFO(url.str()
        << " - "
        << rsp.code);
    }
  }

  return JOINING;
}


GallocyClient::State GallocyClient::state_idle() {
  std::cout << "Idle..." << std::endl;
  return IDLE;
}
