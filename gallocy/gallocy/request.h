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
  explicit Request(gallocy::string raw);
  Request(const Request &) = delete;
  Request &operator=(const Request &) = delete;

  // Members
  Headers headers;
  gallocy::json &get_json();
  Parameters &get_params();
  gallocy::string method;
  gallocy::string uri;
  gallocy::string protocol;
  gallocy::string raw_body;
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
  gallocy::string protocol;
  uint64_t status_code;
  gallocy::string body;

  gallocy::string str();
  uint64_t size();
};

#endif  // GALLOCY_REQUEST_H_
