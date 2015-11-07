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

  typedef gallocy::map<
    gallocy::string, gallocy::string>
    Parameters;

  Headers headers;
  explicit Request(gallocy::string);
  gallocy::json &get_json();
  Parameters &get_params();
  gallocy::string method;
  gallocy::string uri;
  gallocy::string protocol;
  gallocy::string raw_body;

 private:
  gallocy::string raw;
  gallocy::json json;
  Parameters params;
};

#endif  // GALLOCY_REQUEST_H_
