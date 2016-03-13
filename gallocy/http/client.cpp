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


gallocy::http::Response *gallocy::http::CurlClient::request(const gallocy::http::Request &request) {
  RestClient::Response restclient_response;
  if (request.method.compare("GET") == 0) {
    restclient_response = RestClient::get(request.get_url().c_str());
  } else if (request.method.compare("POST") == 0) {
    // TODO(sholsapp): We assume that any POST request is a JSON request, for
    // now. This should be changed to accept any type of POST request.
    restclient_response = RestClient::post(request.get_url().c_str(), "application/json", request.raw_body.c_str());
  } else {
    abort();
  }

  gallocy::http::Response *response = new (internal_malloc(sizeof(gallocy::http::Response))) gallocy::http::Response();
  response->status_code = restclient_response.code;
  for (auto header : restclient_response.headers) {
    response->headers[header.first] = header.second;
  }
  response->body = restclient_response.body.c_str();
  response->peer = request.peer;

  return response;
}


uint64_t gallocy::http::CurlClient::multirequest(const gallocy::vector<gallocy::http::Request> requests,
                                  std::function<bool(const gallocy::http::Response &)> callback,
                                  uint64_t &rsp_count,
                                  std::condition_variable *rsp_have_majority,
                                  std::mutex *rsp_count_lock) {
  int peer_count = requests.size();
  int peer_majority = peer_count / 2;

  std::vector<std::future<uint64_t>> futures;

  for (auto &request : requests) {
    std::future<uint64_t> future =
      std::async(std::launch::async, [&request, &peer_majority, &rsp_count, &rsp_count_lock, &rsp_have_majority, &callback]() {
        gallocy::http::Response *rsp = gallocy::http::CurlClient().request(request);
        uint64_t status_code = rsp->status_code;

        // CHECK if we have a majority of repsonses and signal if ready.
        {
          std::unique_lock<std::mutex> lk(*rsp_count_lock);
          // CHECK if the callback received the expected response.
          if (callback(*rsp))
            rsp_count++;
          if (rsp_count >= peer_majority)
            rsp_have_majority->notify_all();
        }

        // FREE the response since we're done with it.
        rsp->~Response();
        internal_free(rsp);

        // RETURN the HTTP status code.
        return static_cast<uint64_t>(status_code);
      });
    futures.push_back(std::move(future));
  }

  // TODO(sholsapp): Spin off clean up work in a seperate thread. This will
  // require using a shared pointer to the future or something, since detaching
  // here loses references to the future once this function returns.
  for (auto &f : futures) {
    std::future_status status;
    status = f.wait_for(std::chrono::nanoseconds(150));
    if (status == std::future_status::deferred) {
      // no-op
    } else if (status == std::future_status::timeout) {
      // LOG_WARNING("Need to keep waiting... this thread won't be cleaned up!");
    } else if (status == std::future_status::ready) {
      f.get();
    }
  }

  return rsp_count;
}
