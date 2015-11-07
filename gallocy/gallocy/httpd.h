#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <iostream>

#include "libgallocy.h"

#define ISspace(x) isspace((int)(x))

#define SERVER_STRING "Server: gallocy-httpd/0.1.0\r\n"

int get_line(int, char *, int);
void *accept_request(void *);
void cat(int, FILE *);
void error_die(const char *);
void headers(int, const char *);
void serve_file(int, const char *);

void init(void);
void admin(void*);


class HTTPServer {
 public:
  /**
   * Construct a HTTP server.
   */
  explicit HTTPServer(int port) :
    alive(true), port(port), server_socket(-1) {}
  ~HTTPServer() {}
  void start();
  static void *handle_entry(void *arg);
  void *handle(int client);
  void *test(int client_socket);
 private:
  gallocy::string read_request();
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
