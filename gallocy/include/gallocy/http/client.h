#ifndef GALLOCY_HTTP_CLIENT_H_
#define GALLOCY_HTTP_CLIENT_H_

#include <condition_variable>
#include <mutex>
#include <vector>

#include "gallocy/http/request.h"
#include "gallocy/http/response.h"

/**
 * An abstract request client.
 *
 * An abstract request client focuses on HTTP over an underlying transport,
 * which may be TCP, UDP, RDP, or something else. Higher level logic that
 * understands the specifics use of HTTP, e.g., a particular JSON API, should
 * be kept out of this layer. For logic of this type, see \ref GallocyClient.
 */
class AbstractClient {
 public:
  /**
   * Perform a request.
   *
   * \param request The request object which to send.
   * \return A response.
   */
  virtual Response *request(const Request &request) = 0;
  /**
   * Perform many requests.
   *
   * Signal the condition when a majority of responses have successfully been
   * processed by the callback.
   *
   * \param requests The requests objects to send.
   * \param callback The callback function with which to process responses.
   * \param cv The condition to signal when a majority of responses have been processed.
   * \param cv_m The lock assosciated with the condition.
   */
  virtual void multirequest(const gallocy::vector<const Request &> requests,
                            std::function<bool(const Response &)> callback,
                            std::condition_variable *cv,
                            std::mutex *cv_m) = 0;
};


/**
 * A request client that uses a raw TCP transport protocol.
 */
// TODO(sholsapp): Implement me.
class TCPClient : public AbstractClient {
  /**
   * See \ref AbstractClient::request.
   */
  virtual Response *request(const Request &request) = 0;
  /**
   * See \ref AbstractClient::multirequest.
   */
  virtual void multirequest(const gallocy::vector<const Request &> requests,
                            std::function<bool(const Response &)> callback,
                            std::condition_variable *cv,
                            std::mutex *cv_m) = 0;
};


/**
 * A request client that uses a raw UDP transport protocol.
 */
// TODO(sholsapp): Implement me.
class UDPClient : public AbstractClient {
  /**
   * See \ref AbstractClient::request.
   */
  virtual Response *request(const Request &request) = 0;
  /**
   * See \ref AbstractClient::multirequest.
   */
  virtual void multirequest(const gallocy::vector<const Request &> requests,
                            std::function<bool(const Response &)> callback,
                            std::condition_variable *cv,
                            std::mutex *cv_m) = 0;
};


/**
 * A request client that uses a raw RDP transport protocol.
 */
// TODO(sholsapp): Implement me.
class RDPClient : public AbstractClient {
  /**
   * See \ref AbstractClient::request.
   */
  virtual Response *request(const Request &request) = 0;
  /**
   * See \ref AbstractClient::multirequest.
   */
  virtual void multirequest(const gallocy::vector<const Request &> requests,
                            std::function<bool(const Response &)> callback,
                            std::condition_variable *cv,
                            std::mutex *cv_m) = 0;
};


/**
 * A request client that uses restclient-cpp (and cURL) under the hood.
 */
class CurlClient : public AbstractClient {
  /**
   * See \ref AbstractClient::request.
   */
  virtual Response *request(const Request &request) = 0;
  /**
   * See \ref AbstractClient::multirequest.
   */
  virtual void multirequest(const gallocy::vector<const Request &> requests,
                            std::function<bool(const Response &)> callback,
                            std::condition_variable *cv,
                            std::mutex *cv_m) = 0;
};


#endif  // GALLOCY_HTTP_CLIENT_H_
