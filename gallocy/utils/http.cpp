#include <pthread.h>

#include <chrono>
#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "gallocy/allocators/internal.h"
#include "gallocy/utils/http.h"
#include "gallocy/utils/logging.h"
#include "restclient.h"

namespace utils {

// TODO(sholsapp): Use C++ primitives instead of pthread directly.
int get_many(const gallocy::string &path, const gallocy::vector<gallocy::string> &peers, uint16_t port) {
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
#if 0
        uint64_t sleep_duration = 5 - std::rand() % 5;
        LOG_DEBUG("Start std::future to request "
                  << url.c_str()
                  << " after sleeping for "
                  << sleep_duration << " seconds.");
        // For demonstration, sleep for a random period of time <= 5 seconds.
        std::this_thread::sleep_for(std::chrono::seconds(sleep_duration));
#endif
        // Do the actual work.
        RestClient::Response rsp = RestClient::get(url);
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
  while (rsp_count < peer_majority)
    pthread_cond_wait(std::addressof(rsp_have_majority), std::addressof(rsp_count_lock));
  LOG_INFO("Has majority at " << rsp_count << " of " << peer_count << "!");
  pthread_mutex_unlock(std::addressof(rsp_count_lock));

  for (auto &f : futures) {
    std::future_status status;
    status = f.wait_for(std::chrono::nanoseconds(500));
    if (status == std::future_status::deferred) {
      // no-op
    } else if (status == std::future_status::timeout) {
      LOG_WARNING("Need to keep waiting... this thread won't be cleaned up!");
    } else if (status == std::future_status::ready) {
      f.get();
      // LOG_DEBUG("Cleaning up an std::future: " << f.get());
    }
  }

  return rsp_count;
}


int post_many(const gallocy::string &path, const gallocy::vector<gallocy::string> &peers, uint16_t port, gallocy::string json_body, std::function<bool(const RestClient::Response &)> callback) {
  int rsp_count = 0;
  int peer_count = peers.size();
  int peer_majority = peer_count / 2;

  // TODO(sholsapp): Move these to a caller and take them by parameter. The
  // caller can use these to continue operation when a majority of requests are
  // complete.
  std::condition_variable rsp_have_majority;
  std::mutex rsp_count_lock;

  std::vector<std::future<FutureResponse>> futures;

  for (auto peer : peers) {
    std::stringstream s;
    s << "http://" << peer << ":" << port << path;
    std::string url = s.str();
    // Create an async thread.
    std::future<FutureResponse> future =
      std::async(std::launch::async, [url, &peer_majority, &rsp_count, &rsp_count_lock, &rsp_have_majority, &json_body, &callback]() {
#if 0
        uint64_t sleep_duration = 5 - std::rand() % 5;
        LOG_DEBUG("Start std::future to request "
                  << url.c_str()
                  << " after sleeping for "
                  << sleep_duration << " seconds.");
        // For demonstration, sleep for a random period of time <= 5 seconds.
        std::this_thread::sleep_for(std::chrono::seconds(sleep_duration));
#endif
        // Do the actual work.
        RestClient::Response rsp = RestClient::post(url, "application/json", json_body);

        // Check if we have a majority of repsonses and signal if ready.
        {
          std::unique_lock<std::mutex> lk(rsp_count_lock);
          // Check if the callback received the expected response.
          if (callback(rsp))
            rsp_count++;
          if (rsp_count >= peer_majority)
            rsp_have_majority.notify_all();
        }

        // Return the HTTP status code.
        return static_cast<FutureResponse>(rsp.code);
      });
    futures.push_back(std::move(future));
  }

  {
    // LOG_DEBUG("Waiting for majority of threads to respond");
    std::unique_lock<std::mutex> lk(rsp_count_lock);
    while (rsp_count < peer_majority) {
      // TODO(sholsapp): What should time out be such that it doesn't interfere
      // with the leader timeout?
      std::cv_status status = rsp_have_majority.wait_for(lk, std::chrono::milliseconds(50));
      if (status == std::cv_status::timeout) {
        // LOG_DEBUG("Failed to get majority at " << rsp_count << " of " << peer_count << "!");
        return rsp_count;
      } else if (status == std::cv_status::no_timeout) {
        // LOG_DEBUG("Has majority at " << rsp_count << " of " << peer_count << "!");
      }
    }
  }

  for (auto &f : futures) {
    std::future_status status;
    status = f.wait_for(std::chrono::nanoseconds(150));
    if (status == std::future_status::deferred) {
      // no-op
    } else if (status == std::future_status::timeout) {
      // TODO(sholsapp): What should we do here? Are these just zombie threads
      // at this point that never get cleaned up?
      // LOG_WARNING("Need to keep waiting... this thread won't be cleaned up!");
    } else if (status == std::future_status::ready) {
      f.get();
      // LOG_DEBUG("Cleaning up an std::future: " << f.get());
    }
  }

  return rsp_count;
}
}  // namespace utils
