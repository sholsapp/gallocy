#ifndef GALLOCY_RESPONSE_H_
#define GALLOCY_RESPONSE_H_

#include <map>

#include "gallocy/allocators/internal.h"

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

#endif  // GALLOCY_RESPONSE_H_
