#ifndef GALLOCY_HTTP_RESPONSE_H_
#define GALLOCY_HTTP_RESPONSE_H_

#include <map>

#include "gallocy/allocators/internal.h"
#include "gallocy/common/peer.h"

namespace gallocy {

namespace http {

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
  gallocy::common::Peer peer;
  gallocy::string body;
  gallocy::string protocol;
  gallocy::string str();
  uint64_t size();
  uint64_t status_code;
};

}  // namespace http

}  // namespace gallocy

#endif  // GALLOCY_HTTP_RESPONSE_H_
