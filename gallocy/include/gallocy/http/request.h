#ifndef GALLOCY_REQUEST_H_
#define GALLOCY_REQUEST_H_

#include <map>

#include "gallocy/allocators/internal.h"

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

#endif  // GALLOCY_REQUEST_H_

