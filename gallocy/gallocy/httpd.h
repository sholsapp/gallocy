#ifndef GALLOCY_HTTPD_H_
#define GALLOCY_HTTPD_H_

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

#include "gallocy/http_router.h"
#include "gallocy/libgallocy.h"
#include "gallocy/request.h"


void error_die(const char *);


class HTTPServer {
 public:
  using RouteArguments = gallocy::vector<gallocy::string>;
  using HandlerFunction = std::function<int(RouteArguments *)>;
 public:
  /**
   * Construct a HTTP server.
   */
  explicit HTTPServer(int port) :
    alive(true), port(port), server_socket(-1) {
      routes.register_handler("/admin", [this](RouteArguments *args) { return route_admin(args); });
  }
  ~HTTPServer() {}
  void start();
  static void *handle_entry(void *arg);
  void *handle(int client_socket);
  Request *get_request(int client_socket);
  RoutingTable<HandlerFunction> routes;

  int route_admin(RouteArguments *args);

 private:
  int get_line(int client_socket, gallocy::stringstream &line);
 private:
  bool alive;
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
  HTTPServer *server;
  int client_socket;
};

#endif  // GALLOCY_HTTPD_H_
