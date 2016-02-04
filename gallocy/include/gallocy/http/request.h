#ifndef GALLOCY_HTTP_REQUEST_H_
#define GALLOCY_HTTP_REQUEST_H_

#include <map>

#include "gallocy/allocators/internal.h"

/**
 * A HTTP request.
 */
class Request {
 public:
  /**
   * HTTP header type.
   */
  typedef gallocy::map<
    gallocy::string, gallocy::string>
    Headers;
  /**
   * HTTP parameter type.
   */
  typedef gallocy::map<
    gallocy::string, gallocy::string>
    Parameters;
  /**
   * Create a request.
   *
   * This constructor is suitable for creating a request object from a raw
   * string that was read off the wire, e.g., in a server implementation.
   *
   * \param raw The raw request.
   */
  explicit Request(gallocy::string raw);
  /**
   * Create a request.
   *
   * This constructor is suitable for creating a request from scratch, e.g.,
   * in a client implementation.
   *
   * \param method An HTTP method, such as GET or POST.
   * \param peer_ip The peer's internet address.
   * \param uri The URI to request.
   */
  Request(gallocy::string method, gallocy::string peer, gallocy::string uri);
  /**
   * Create a request.
   *
   * This constructor is suitable for creating a request from scratch, e.g.,
   * in a client implementation.
   *
   * \param method An HTTP method, such as GET or POST.
   * \param peer_ip The peer's internet address.
   * \param uri The URI to request.
   * \param body The raw request body.
   * \param headers Any headers to include.
   */
  Request(gallocy::string method, gallocy::string peer, gallocy::string uri,
          gallocy::string body, Headers headers);

  // Request(const Request &) = delete;
  // Request &operator=(const Request &) = delete;

  // Members
  Headers headers;
  Parameters &get_params();
  gallocy::json &get_json();
  gallocy::string method;
  gallocy::string protocol;
  gallocy::string raw_body;
  gallocy::string uri;
  gallocy::string peer;
  uint64_t peer_ip;
  void pretty_print() const;

 private:
  gallocy::string raw;
  gallocy::json json;
  Parameters params;
};

#endif  // GALLOCY_HTTP_REQUEST_H_
