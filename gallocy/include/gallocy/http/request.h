#ifndef GALLOCY_REQUEST_H_
#define GALLOCY_REQUEST_H_

#include <map>

#include "gallocy/libgallocy.h"

/**
 * A HTTP request.
 */
class Request {
 public:
  // Types
  typedef gallocy::map<
    gallocy::string, gallocy::string>
    Headers;

  typedef gallocy::map<
    gallocy::string, gallocy::string>
    Parameters;

  // Constructors
  // TODO(sholsapp) We need to pass in the client's socket name here so we can
  // reason about the client, if required, e.g., in consensus module.
  explicit Request(gallocy::string raw);
  Request(const Request &) = delete;
  Request &operator=(const Request &) = delete;

  // TODO(sholsapp): Here be dragons: whenever I change the ordering of these
  // definitions we get sporadic failures, segfault, wtf.

  // Members
  Headers headers;
  Parameters &get_params();
  gallocy::json &get_json();
  gallocy::string method;
  gallocy::string protocol;
  gallocy::string raw_body;
  gallocy::string uri;
  uint64_t peer_ip;

  void pretty_print();

 private:
  gallocy::string raw;
  gallocy::json json;
  Parameters params;
};


/**
 * A HTTP response.
 */
class Response {
 public:
  // Types
  typedef gallocy::map<
    gallocy::string, gallocy::string>
    Headers;

  // Constructors
  Response();
  Response(const Response&) = delete;
  Response &operator=(const Response&) = delete;

  // Members
  Headers headers;
  gallocy::string body;
  gallocy::string protocol;
  uint64_t peer_ip;
  uint64_t status_code;

  gallocy::string str();
  uint64_t size();
};

#endif  // GALLOCY_REQUEST_H_
