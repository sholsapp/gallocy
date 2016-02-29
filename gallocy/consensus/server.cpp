#include <ctime>
#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "gallocy/consensus/client.h"
#include "gallocy/consensus/log.h"
#include "gallocy/consensus/server.h"
#include "gallocy/entrypoint.h"
#include "gallocy/http/request.h"
#include "gallocy/http/response.h"
#include "gallocy/models.h"
#include "gallocy/threads.h"
#include "gallocy/utils/logging.h"
#include "gallocy/utils/stringutils.h"


/**
 * Die.
 *
 * :param sc: A custom error to pass alont to ``perror``.
 */
void error_die(const char *sc) {
  perror(sc);
  exit(1);
}


gallocy::http::Response *GallocyServer::route_admin(RouteArguments *args, gallocy::http::Request *request) {
  gallocy::http::Response *response = new (internal_malloc(sizeof(gallocy::http::Response))) gallocy::http::Response();
  response->status_code = 200;
  response->headers["Server"] = "Gallocy-Httpd";
  response->headers["Content-Type"] = "application/json";
  response->body = "GOOD";
  args->~RouteArguments();
  internal_free(args);
  return response;
}


gallocy::http::Response *GallocyServer::route_request_vote(RouteArguments *args, gallocy::http::Request *request) {
  gallocy::common::Peer peer = request->peer;
  gallocy::json request_json = request->get_json();
  uint64_t candidate_commit_index = request_json["commit_index"];
  uint64_t candidate_current_term = request_json["term"];
  uint64_t candidate_last_applied = request_json["last_applied"];
  gallocy::common::Peer candidate_voted_for = peer;
  uint64_t local_commit_index = gallocy_state->get_commit_index();
  uint64_t local_current_term = gallocy_state->get_current_term();
  uint64_t local_last_applied = gallocy_state->get_last_applied();
  gallocy::common::Peer local_voted_for = gallocy_state->get_voted_for();
  bool granted = false;

  if (candidate_current_term < local_current_term) {
    granted = false;
  } else if (local_voted_for == gallocy::common::Peer()
      || local_voted_for == candidate_voted_for) {
    if (candidate_last_applied >= local_last_applied
        && candidate_commit_index >= local_commit_index) {
      LOG_INFO("Granting vote to "
          << candidate_voted_for.get_string()
          << " in term " << candidate_current_term);

      gallocy_state->set_current_term(candidate_current_term);
      gallocy_state->set_voted_for(candidate_voted_for);
      gallocy_state->get_timer()->reset();
      granted = true;
    } else {
      // TODO(sholsapp): Implement logic to reject candidates that don't have
      // an up to date log.
      LOG_ERROR("Handling of out-of-date log is unimplemented");
    }
  }
  gallocy::json response_json = {
    // TODO(sholsapp): This information should from from the socket, not the
    // payload, as it can be faked.
    { "peer", gallocy_config->address.c_str() },
    { "term", gallocy_state->get_current_term() },
    { "vote_granted", granted },
  };
  gallocy::http::Response *response = new (internal_malloc(sizeof(gallocy::http::Response))) gallocy::http::Response();
  response->headers["Server"] = "Gallocy-Httpd";
  response->headers["Content-Type"] = "application/json";
  response->status_code = 200;
  response->body = response_json.dump();
  args->~RouteArguments();
  internal_free(args);
  return response;
}


gallocy::http::Response *GallocyServer::route_append_entries(RouteArguments *args, gallocy::http::Request *request) {
  gallocy::common::Peer peer = request->peer;
  gallocy::json request_json = request->get_json();
  gallocy::vector<LogEntry> leader_entries;
  uint64_t leader_commit_index = request_json["leader_commit"];
  uint64_t leader_prev_log_index = request_json["previous_log_index"];
  uint64_t leader_prev_log_term = request_json["previous_log_term"];
  uint64_t leader_term = request_json["term"];
  uint64_t local_term = gallocy_state->get_current_term();
  bool success = false;

  // Decode log entries from JSON payload
  for (auto entry_json : request_json["entries"]) {
    // TODO(sholsapp): Implicit conversion issue.
    gallocy::string tmp = entry_json["command"];
    uint64_t term = entry_json["term"];
    Command command(tmp);
    LogEntry entry(command, term);
    leader_entries.push_back(entry);
  }

  if (leader_term < local_term) {
    LOG_INFO("Rejecting leader "
        << peer.get_string()
        << " because term is outdated ("
        << leader_term
        << ")");
    success = false;
  } else {
    if (leader_entries.size() > 0) {
      LOG_INFO("Need to append " << leader_entries.size() << " new entries!");
      // TODO(sholsapp): Use state object to append entries to the log, and
      // report meaningful results on failure.
    }
    success = true;
    gallocy_state->set_current_term(leader_term);
    gallocy_state->set_state(RaftState::FOLLOWER);
    gallocy_state->set_voted_for(peer);
    gallocy_state->get_timer()->reset();
  }
  gallocy::json response_json = {
    // TODO(sholsapp): This information should from from the socket, not the
    // payload, as it can be faked.
    { "peer", gallocy_config->address.c_str() },
    { "term", gallocy_state->get_current_term() },
    { "success", success },
  };
  gallocy::http::Response *response = new (internal_malloc(sizeof(gallocy::http::Response))) gallocy::http::Response();
  response->headers["Server"] = "Gallocy-Httpd";
  response->headers["Content-Type"] = "application/json";
  response->status_code = 200;
  response->body = response_json.dump();
  args->~RouteArguments();
  internal_free(args);
  return response;
}


// TODO(sholsapp): This is just a route that we can hit to trigger an append
// entries action. Once we're done testing, we can remove this route.
gallocy::http::Response *GallocyServer::route_request(RouteArguments *args, gallocy::http::Request *request) {
  Command command("hello world");
  LogEntry entry(command, gallocy_state->get_current_term());
  gallocy::vector<LogEntry> entries;
  entries.push_back(entry);

  gallocy_client->send_append_entries(entries);

  gallocy::http::Response *response = new (internal_malloc(sizeof(gallocy::http::Response))) gallocy::http::Response();
  response->headers["Server"] = "Gallocy-Httpd";
  response->headers["Content-Type"] = "application/json";
  response->status_code = 200;
  response->body = "GOOD";
  args->~RouteArguments();
  internal_free(args);
  return response;
}


// TODO(rverdon): This needs an abstraction so that we can swap out the
// underlying transport mechanism for the server *at the same time* as the
// client. What about libevent, here?

//
// The server socket implementation
//


void *GallocyServer::work() {
  LOG_DEBUG("Starting HTTP server on " << address << ":" << port);

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
  // name.sin_addr.s_addr = htonl(INADDR_ANY);
  name.sin_addr.s_addr = inet_addr(address.c_str());

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
    ctx->client_name = client_name;

    if (get_pthread_create_impl()(&newthread, NULL, handle_entry, reinterpret_cast<void *>(ctx)) != 0) {
      perror("pthread_create1");
    }

    // TODO(sholsapp): This shouldn't block, and we shouldn't just try to
    // join this thread.
    if (get_pthread_join_impl()(newthread, nullptr)) {
      perror("pthread_join1");
    }
  }

  close(server_socket);

  return nullptr;
}


void *GallocyServer::handle_entry(void *arg) {
  struct RequestContext *ctx = reinterpret_cast<struct RequestContext *>(arg);
  void *ret = ctx->server->handle(ctx->client_socket, ctx->client_name);
  ctx->~RequestContext();
  internal_free(ctx);
  return ret;
}


void *GallocyServer::handle(int client_socket, struct sockaddr_in client_name) {
  gallocy::http::Request *request = get_request(client_socket);
  gallocy::http::Response *response = routes.match(request->uri)(request);

  if (send(client_socket, response->str().c_str(), response->size(), 0) == -1) {
    error_die("send");
  }

  LOG_INFO(request->method
    << " "
    << request->uri
    << " - "
    << "HTTP " << response->status_code
    << " - "
    << inet_ntoa(client_name.sin_addr)
    << " "
    << request->headers["User-Agent"]);

  // Teardown
  request->~Request();
  internal_free(request);
  response->~Response();
  internal_free(response);

  shutdown(client_socket, SHUT_RDWR);
  close(client_socket);

  return nullptr;
}


gallocy::http::Request *GallocyServer::get_request(int client_socket) {
  gallocy::stringstream request;
  int n;
  char buf[512] = {0};
  n = recv(client_socket, buf, 16, 0);
  request << buf;
  while (n > 0) {
    memset(buf, 0, 512);
    n = recv(client_socket, buf, 512, MSG_DONTWAIT);
    request << buf;
  }
  return new (internal_malloc(sizeof(gallocy::http::Request))) gallocy::http::Request(request.str());
}
