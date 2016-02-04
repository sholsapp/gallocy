#include <condition_variable>
#include <future>
#include <mutex>
#include <thread>
#include <map>
#include <vector>

#include "gallocy/allocators/internal.h"
#include "gallocy/http/client.h"
#include "gallocy/http/request.h"
#include "gallocy/http/response.h"
#include "restclient.h"  // NOLINT


Response *CurlClient::request(const Request &request) {
  RestClient::Response restclient_response;

  gallocy::stringstream url;

  // TODO(sholsapp): This port needs to be exposed so we can build the URL.
  // Perhaps we need a Peer abstraction in the request object?
  url << "http://" << request.peer << ":" << "8080"
      << request.uri;

  if (request.method.compare("GET") == 0) {
    restclient_response = RestClient::get(url.str().c_str());
  } else if (request.method.compare("POST") == 0) {
    // TODO(sholsapp): We assume that any POST request is a JSON request, for
    // now. This should be changed to accept any type of POST request.
    restclient_response = RestClient::post(url.str().c_str(), "application/json", request.raw_body.c_str());
  } else {
    abort();
  }

  Response *response = new (internal_malloc(sizeof(Response))) Response();
  response->status_code = restclient_response.code;
  for (auto header : restclient_response.headers) {
    response->headers[header.first] = header.second;
  }
  response->body = restclient_response.body.c_str();

  return response;
}


uint64_t CurlClient::multirequest(const gallocy::vector<Request> requests,
                                  std::function<bool(const Response &)> callback,
                                  std::condition_variable *cv,
                                  std::mutex *cv_m) {
  int rsp_count = 0;
  int peer_count = requests.size();
  int peer_majority = peer_count / 2;

  // TODO(sholsapp): Move these to a caller and take them by parameter. The
  // caller can use these to continue operation when a majority of requests are
  // complete.
  std::condition_variable rsp_have_majority;
  std::mutex rsp_count_lock;

  std::vector<std::future<uint64_t>> futures;

  for (auto &request : requests) {
    std::future<uint64_t> future =
      std::async(std::launch::async, [&request, &peer_majority, &rsp_count, &rsp_count_lock, &rsp_have_majority, &callback]() {
        Response *rsp = CurlClient().request(request);
        uint64_t status_code = rsp->status_code;

        // CHECK if we have a majority of repsonses and signal if ready.
        {
          std::unique_lock<std::mutex> lk(rsp_count_lock);
          // CHECK if the callback received the expected response.
          if (callback(*rsp))
            rsp_count++;
          if (rsp_count >= peer_majority)
            rsp_have_majority.notify_all();
        }

        // FREE the response since we're done with it.
        rsp->~Response();
        internal_free(rsp);

        // RETURN the HTTP status code.
        return static_cast<uint64_t>(status_code);
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
