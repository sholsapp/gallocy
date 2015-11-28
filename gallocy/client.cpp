#include "gallocy/client.h"

#include <unistd.h>
#include <pthread.h>

#include <iostream>


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
        std::cout << "Joining..." << std::endl;
        break;
      case IDLE:
        std::cout << "Idle..." << std::endl;
        for (auto i : peers)
          std::cout << i << std::endl;
        break;
      default:
        std::cout << "Default..." << std::endl;
        break;
    }
    sleep(5);
  }
  return nullptr;
}
