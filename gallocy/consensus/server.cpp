#include <ctime>
#include <functional>
#include <map>
#include <string>
#include <utility>

#include "gallocy/consensus/server.h"
#include "gallocy/entrypoint.h"
#include "gallocy/logging.h"
#include "gallocy/models.h"
#include "gallocy/request.h"
#include "gallocy/stringutils.h"
#include "gallocy/threads.h"


/**
 * Die.
 *
 * :param sc: A custom error to pass alont to ``perror``.
 */
void error_die(const char *sc) {
  perror(sc);
  exit(1);
}


Response *GallocyServer::route_admin(RouteArguments *args, Request *request) {
  Response *response = new (internal_malloc(sizeof(Response))) Response();
  response->status_code = 200;
  response->headers["Server"] = "Gallocy-Httpd";
  response->headers["Content-Type"] = "application/json";

  gallocy::json j = {
    {"status", "GOOD" },
    {"master", is_master},
    {"peers", { } },
    {"diagnostics", {
      // TODO(sholsapp): These names suck.. we should indicate that they are allocators.
      { "local_internal_memory", reinterpret_cast<uint64_t>(&local_internal_memory) },
      { "shared_page_table", reinterpret_cast<uint64_t>(&shared_page_table) },
      // TODO(sholsapp): Add the main application allocator here.
    } },
  };

  for (auto p : peer_info_table.all()) {
    gallocy::json peer_info = {
      {"id", p.id},
      {"ip_address", p.ip_address},
      {"first_seen", p.first_seen},
      {"last_seen", p.last_seen},
    };
    j["peers"].push_back(peer_info);
  }

  response->body = j.dump();

  args->~RouteArguments();
  internal_free(args);

  return response;
}


Response *GallocyServer::route_join(RouteArguments *args, Request *request) {
  Response *response = new (internal_malloc(sizeof(Response))) Response();
  // TODO(sholsapp): Setup the joining logic here.

  gallocy::json peer_info = request->get_json();

  // TODO(sholsapp): We need a serialization framework so that we're not doing
  // random JSON building/parsing. Maybe the models should have a codec build
  // into them?
  int ret = e.execute(
    PeerInfo(
      utils::parse_internet_address(peer_info["ip_address"]),
      std::time(nullptr),
      std::time(nullptr),
      static_cast<gallocy::json::boolean_t>(peer_info["is_master"])).insert());

  response->headers["Server"] = "Gallocy-Httpd";
  response->headers["Content-Type"] = "application/json";

  if (ret) {
    response->status_code = 200;
    gallocy::json j = {
      {"status", "JOINED" },
    };
    response->body = j.dump();
  } else {
    response->status_code = 500;
    response->body = "";
  }

  args->~RouteArguments();
  internal_free(args);

  return response;
}


Response *GallocyServer::route_request_vote(RouteArguments *args, Request *request) {
  gallocy::json request_json = request->get_json();
  uint64_t candidate_commit_index = request_json["commit_index"];
  uint64_t candidate_current_term = request_json["current_term"];
  uint64_t candidate_last_applied = request_json["last_applied"];
  uint64_t candidate_voted_for = request->peer_ip;
  uint64_t local_commit_index = gallocy_state->get_commit_index();
  uint64_t local_current_term = gallocy_state->get_current_term();
  uint64_t local_last_applied = gallocy_state->get_last_applied();
  uint64_t local_voted_for = gallocy_state->get_voted_for();
  bool granted = false;

  if (candidate_current_term < local_current_term) {
    // This candidate is in an old term and thus does not earn a vote from us.
    granted = false;
  } else if (local_voted_for == 0
      || local_voted_for == candidate_voted_for) {

    if (candidate_last_applied >= local_last_applied
        && candidate_commit_index >= local_commit_index) {

      LOG_INFO("Granting vote to "
          << candidate_voted_for
          << " in term " << candidate_current_term);

      gallocy_state->set_current_term(candidate_current_term);
      gallocy_state->set_voted_for(candidate_voted_for);
      gallocy_state->reset_timer();

      // This candidate is in current term, is eligable for our vote, and is at
      // least as up to date as us.
      granted = true;
    } else {
      LOG_ERROR("This is an odd candidate state to be in and *may* be a logic error.");
    }
  }

  gallocy::json response_json = {
    { "peer", gallocy_config->address.c_str() },
    { "term", gallocy_state->get_current_term() },
    { "vote_granted", granted },
  };
  Response *response = new (internal_malloc(sizeof(Response))) Response();
  response->headers["Server"] = "Gallocy-Httpd";
  response->headers["Content-Type"] = "application/json";
  response->status_code = 200;
  response->body = response_json.dump();
  args->~RouteArguments();
  internal_free(args);
  return response;
}


Response *GallocyServer::route_append_entries(RouteArguments *args, Request *request) {
  gallocy::json request_json = request->get_json();
  uint64_t leader_term = request_json["current_term"];
  uint64_t local_term = gallocy_state->get_current_term();
  bool success = false;

  if (leader_term < local_term) {
    LOG_INFO("Rejecting leader "
        << request->peer_ip
        << " because term is outdated ("
        << leader_term
        << ")");
    success = false;
  } else {
    success = true;
    gallocy_state->set_current_term(leader_term);
    gallocy_state->set_state(RaftState::FOLLOWER);
    gallocy_state->set_voted_for(request->peer_ip);
    gallocy_state->reset_timer();
  }
  gallocy::json response_json = {
    { "peer", gallocy_config->address.c_str() },
    { "term", gallocy_state->get_current_term() },
    { "success", success },
  };
  Response *response = new (internal_malloc(sizeof(Response))) Response();
  response->headers["Server"] = "Gallocy-Httpd";
  response->headers["Content-Type"] = "application/json";
  response->status_code = 200;
  response->body = response_json.dump();
  args->~RouteArguments();
  internal_free(args);
  return response;
}


void *GallocyServer::work() {
  LOG_INFO("Starting HTTP server on " << address << ":" << port);

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

    if (__gallocy_pthread_create(&newthread, NULL, handle_entry, reinterpret_cast<void *>(ctx)) != 0) {
      perror("pthread_create1");
    }

    // TODO(sholsapp): This shouldn't block, and we shouldn't just try to
    // join this thread.
    if (__gallocy_pthread_join(newthread, nullptr)) {
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
  Request *request = get_request(client_socket);
  request->peer_ip = utils::parse_internet_address(inet_ntoa(client_name.sin_addr));
  Response *response = routes.match(request->uri)(request);

  if (send(client_socket, response->str().c_str(), response->size(), 0) == -1) {
    error_die("send");
  }

  LOG_INFO(request->method
    << " "
    << request->uri
    << " - "
    << "HTTP " << response->status_code
    << " - "
    << inet_ntoa(client_name.sin_addr) << " (" << request->peer_ip << ")" << " "
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


Request *GallocyServer::get_request(int client_socket) {
  gallocy::stringstream request;
  int n;
  char buf[256] = {0};
  n = recv(client_socket, buf, 16, 0);
  request << buf;
  while (n > 0) {
    memset(buf, 0, 256);
    n = recv(client_socket, buf, 256, MSG_DONTWAIT);
    request << buf;
  }
  return new (internal_malloc(sizeof(Request))) Request(request.str());
}
