#ifndef GALLOCY_REQUEST_H_
#define GALLOCY_REQUEST_H_

#include <map>

#include "./libgallocy.h"

/**
 * A HTTP request.
 */
class Request {
 public:
  /**
   * HTTP headers are a simple string to string map.
   */
  typedef gallocy::map<
    gallocy::string, gallocy::string>
    Headers;

  explicit Request(gallocy::string);
  gallocy::string method;
  gallocy::string path;
  gallocy::string protocol;
  Headers headers;
  gallocy::string raw_body;

 private:
  gallocy::string raw;
};

#endif  // GALLOCY_REQUEST_H_
