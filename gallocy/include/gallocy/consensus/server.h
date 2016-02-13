#ifndef GALLOCY_SERVER_H_
#define GALLOCY_SERVER_H_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <functional>
#include <vector>

#include "gallocy/http_router.h"
#include "gallocy/request.h"
#include "gallocy/utils/config.h"
#include "gallocy/worker.h"


void error_die(const char *);


class GallocyServer : public ThreadedDaemon {
 public:
  using RouteArguments = gallocy::vector<gallocy::string>;
  using HandlerFunction = std::function<Response *(RouteArguments *, Request *)>;
  /**
   * Construct a HTTP server.
   *
   * Creates, but does not start, a HTTP server. There can exist only one of
   * these objects in a well formed application and it is not copyable.
   *
   * :param config: A gallocy configuration object.
   */
  explicit GallocyServer(GallocyConfig &config) :
    config(config),
    is_master(config.master),
    address(config.address),
    port(config.port),
    server_socket(-1) {
      routes.register_handler("/admin",
        [this](RouteArguments *args, Request *request) { return route_admin(args, request); });
      routes.register_handler("/raft/request_vote",
        [this](RouteArguments *args, Request *request) { return route_request_vote(args, request); });
      routes.register_handler("/raft/append_entries",
        [this](RouteArguments *args, Request *request) { return route_append_entries(args, request); });
  }
  GallocyServer(const GallocyServer &) = delete;
  GallocyServer &operator=(const GallocyServer &) = delete;
  ~GallocyServer() {}
  /**
   * Read an HTTP request from a socket.
   *
   * :param client_socket: The client's socket id.
   * :param request: The string stream to write the request into.
   * :returns: The request object.
   */
  Request *get_request(int client_socket);
  /**
   * A static helper for handling requests.
   *
   * This static helper is for use with pthreads and extracts a
   * :class:`RequestContext` pointer from the void pointer argument. When the
   * request is done being handled, the :class:``RequestContext`` should be
   * freed.
   *
   * :param arg: A heap``RequestContext`` argument.
   * :returns: A null pointer.
   */
  static void *handle_entry(void *arg);
  /**
   * Handle a HTTP request.
   *
   * The handling of the HTTP request is done in a threaded context. Access to
   * the server resources is available, but must be synchronized.
   *
   * The route handler of the HTTP request is done by matching the HTTP request's
   * URI against the map of registered routes. The route handler will be called
   * with the URI arguments and the request object itself. The route handler is
   * responsible for managing memory for all parameters passed to it.
   *
   * :param client_socket: The client's socket id.
   * :returns: A null pointer.
   */
  void *handle(int client_socket, struct sockaddr_in client_name);
  /**
   * The primary work loop.
   *
   * Starting the HTTP server binds to the socket, begins listening on the bound
   * socket, then enters the HTTP server's main event loop. The HTTP server's
   * main event loop listens while alive for incoming connections and creates a
   * handler thread for each established connection.
   *
   * To stop the HTTP server set `alive` to false.
   */
  void *work();
  /**
   * Routing table for routes known to this server.
   *
   * See :class:`RoutingTable` for additional details.
   */
  RoutingTable<HandlerFunction> routes;
  /**
   * Handle a request for /admin.
   *
   * :param args: The route arguments.
   * :param request: The request itself.
   */
  Response *route_admin(RouteArguments *args, Request *request);
  /**
   * Handle a request for /raft/request_vote.
   *
   * :param args: The route arguments.
   * :param request: The request itself.
   */
  Response *route_request_vote(RouteArguments *args, Request *request);
  /**
   * Handle a request for /raft/append_entries.
   *
   * :param args: The route arguments.
   * :param request: The request itself.
   */
  Response *route_append_entries(RouteArguments *args, Request *request);

 private:
  GallocyConfig &config;
  bool is_master;
  gallocy::string address;
  int16_t port;
  int64_t server_socket;
};


/**
 * A simple class to bundle server and socket together.
 *
 * This abstraction is for use with pthreads, which consumes a void pointer as
 * its only argument.
 */
struct RequestContext {
 public:
  GallocyServer *server;
  int client_socket;
  struct sockaddr_in client_name;
};

#endif  // GALLOCY_SERVER_H_
