#include <pthread.h>

#include <iostream>
#include <future>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <sstream>

#include "gallocy/allocators/internal.h"
#include "gallocy/logging.h"
#include "gallocy/utils/http.h"
#include "restclient.h"

namespace utils {

int multirequest(const gallocy::string &path, const gallocy::vector<gallocy::string> &peers, uint16_t port) {
  int rsp_count = 0;
  int peer_count = peers.size();
  int peer_majority = peer_count / 2;

  // TODO(sholsapp): Move these to a caller and take them by parameter. The
  // caller can use these to continue operation when a majority of requests are
  // complete.
  pthread_mutex_t rsp_count_lock = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t rsp_have_majority = PTHREAD_COND_INITIALIZER;
  std::vector<std::future<FutureResponse>> futures;

  for (auto peer : peers) {
    std::stringstream s;
    // TODO(sholsapp): Expose the path to fetch, otherwise this is worthless.
    s << "http://" << peer << ":" << port << path;
    std::string url = s.str();
    // Create an async thread.
    std::future<FutureResponse> future =
      std::async(std::launch::async, [url, &peer_majority, &rsp_count, &rsp_count_lock, &rsp_have_majority]() {
        uint64_t sleep_duration = 5 - std::rand() % 5;
        LOG_DEBUG("Start std::future to request "
                  << url.c_str()
                  << " after sleeping for "
                  << sleep_duration << " seconds.");
        // For demonstration, sleep for a random period of time <= 5 seconds.
        std::this_thread::sleep_for(std::chrono::seconds(sleep_duration));
        // Do the actual work.
        RestClient::response rsp = RestClient::get(url);
        // Check if we have a majority of repsonses and signal if ready.
        pthread_mutex_lock(std::addressof(rsp_count_lock));
        rsp_count++;
        if (rsp_count >= peer_majority)
          pthread_cond_signal(std::addressof(rsp_have_majority));
        pthread_mutex_unlock(std::addressof(rsp_count_lock));
        // Return the HTTP status code.
        return static_cast<FutureResponse>(rsp.code);
      });
    futures.push_back(std::move(future));
  }

  pthread_mutex_lock(std::addressof(rsp_count_lock));
  while (rsp_count <= peer_majority)
    pthread_cond_wait(std::addressof(rsp_have_majority), std::addressof(rsp_count_lock));
  LOG_INFO("Has majority at " << rsp_count << " of " << peer_count << "!");
  pthread_mutex_unlock(std::addressof(rsp_count_lock));

  for (auto &f : futures) {
    std::future_status status;
    status = f.wait_for(std::chrono::nanoseconds(500));
    if (status == std::future_status::deferred) {
      // no-op
    } else if (status == std::future_status::timeout) {
      LOG_DEBUG("Need to keep waiting...");
    } else if (status == std::future_status::ready) {
      LOG_DEBUG("Cleaning up an std::future: " << f.get());
    }
  }

  return 0;
}

}  // namespace utils
