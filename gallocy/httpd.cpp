#include <functional>
#include <map>
#include <string>
#include <utility>

#include "gallocy/httpd.h"
#include "gallocy/libgallocy.h"
#include "gallocy/request.h"
#include "gallocy/stringutils.h"


typedef
  std::map<std::string, void (*)(void*),
  std::less<std::string>,
  STLAllocator<std::pair<std::string, void (*)(void*)>, SingletonHeapType> >
    routing_table_t;


routing_table_t routing_table;


void init() {
  routing_table["/admin"] = &admin;
}


void admin(void* arg) {
  std::cout << "ADMIN" << std::endl;
}

/**
 * Die.
 *
 * :param sc: A custom error to pass alont to ``perror``.
 */
void error_die(const char *sc) {
  perror(sc);
  exit(1);
}


/**
 * Get a line from a socket.
 *
 * Lines end in a newline, a carriage return, or a CRLF combination. This
 * function terminates the string read with a null character. If no newline
 * indicator is found before the end of the buffer, the string is terminated
 * with a null. If any of the above three line terminators is read, the last
 * character of the string will be a linefeed and the string will be terminated
 * with a null character.
 *
 * :param client_socket: The socket descriptor.
 * :param line: A string stream to write to.
 * :returns: The number of bytes stored.
 */
int HTTPServer::get_line(int client_socket, gallocy::stringstream &line) {
  int i = 0;
  char c = '\0';
  int n;
  while (c != '\n') {
    n = recv(client_socket, &c, 1, 0);
    if (n > 0) {
      if (c == '\r') {
        n = recv(client_socket, &c, 1, MSG_PEEK);
        if ((n > 0) && (c == '\n')) {
          recv(client_socket, &c, 1, 0);
        } else {
          c = '\n';
        }
      }
      line << c;
      i++;
    } else {
      c = '\n';
    }
  }
  // This angers C++ streams.
  // line << '\0';
  return(i);
}


/**
 * Start the HTTP server.
 *
 * Starting the HTTP server binds to the socket, begins listening on the bound
 * socket, then enters the HTTP server's main event loop.
 */
void HTTPServer::start() {
  std::cout << "Starting the HTTP sever..." << std::endl;

  struct sockaddr_in name;
  int optval = 1;

  server_socket = socket(PF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    error_die("socket");
  }

#ifdef __APPLE__
  setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
#else
  setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
#endif

  memset(&name, 0, sizeof(name));
  name.sin_family = AF_INET;
  name.sin_port = htons(port);
  name.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(server_socket, (struct sockaddr *) &name, sizeof(name)) < 0) {
    error_die("bind");
  }

  if (listen(server_socket, 5) < 0) {
    error_die("listen");
  }

  int64_t client_sock = -1;
  struct sockaddr_in client_name;
  uint64_t client_name_len = sizeof(client_name);
  pthread_t newthread;

  while (alive) {
    client_sock = accept(server_socket,
        reinterpret_cast<struct sockaddr *>(&client_name),
        reinterpret_cast<socklen_t *>(&client_name_len));

    if (client_sock == -1) {
      error_die("accept");
    }

    struct RequestContext *ctx =
      new (internal_malloc(sizeof(struct RequestContext))) struct RequestContext;
    ctx->server = this;
    ctx->client_socket = client_sock;

    if (pthread_create(&newthread, NULL, handle_entry, reinterpret_cast<void *>(ctx)) != 0) {
      perror("pthread_create1");
    }

    // TODO(sholsapp): This shouldn't block, and we shouldn't just try to
    // join this thread.
    if (pthread_join(newthread, nullptr)) {
      perror("pthread_join1");
    }
  }

  close(server_socket);

  return;
}


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
void *HTTPServer::handle_entry(void *arg) {
  struct RequestContext *ctx = reinterpret_cast<struct RequestContext *>(arg);
  void *ret = ctx->server->handle(ctx->client_socket);
  internal_free(ctx);
  return ret;
}


/**
 * Handle a HTTP request.
 *
 * The handling of the HTTP request is done in a threaded context. Access to
 * the server resources is available, but must be synchronized.
 *
 * :param client_socket: The client's socket id.
 * :returns: A null pointer.
 */
void *HTTPServer::handle(int client_socket) {
  std::cout << "Server (" << this
            << ") is servering client on " << client_socket
            << std::endl;

  Request *req = get_request(client_socket);

  req->pretty_print();

  // TODO(sholsapp): Figure out routing information here and route to
  // appropriate handler function.

  Response response;
  response.status_code = 200;
  response.headers["Server"] = "Gallocy-Httpd";
  response.headers["Content-Type"] = "application/json";
  gallocy::json j = { { "foo", "bar" } };
  // There is no known conversion from std::string to
  // gallocy::string... we should fix this.
  response.body = j.dump().c_str();

  send(client_socket, response.str().c_str(), response.size(), 0);

  // Teardown
  internal_free(req);

  shutdown(client_socket, SHUT_RDWR);
  close(client_socket);

  return nullptr;
}


/**
 * Read an HTTP request from a socket.
 *
 * :param client_socket: The client's socket id.
 * :param request: The string stream to write the request into.
 * :returns: The request object.
 */
Request *HTTPServer::get_request(int client_socket) {
  int numchars = 0;
  int numlines = 0;
  gallocy::stringstream stream;
  while ((numchars = get_line(client_socket, stream)) > 2) {
    if (numchars > 0)
      numlines++;
  }
  return new (internal_malloc(sizeof(Request))) Request(stream.str());
}
