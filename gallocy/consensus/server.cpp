#include <ctime>
#include <functional>
#include <map>
#include <string>
#include <utility>

#include "gallocy/consensus/server.h"
#include "gallocy/entrypoint.h"
#include "gallocy/http/request.h"
#include "gallocy/models.h"
#include "gallocy/threads.h"
#include "gallocy/utils/http.h"
#include "gallocy/utils/logging.h"
#include "gallocy/utils/stringutils.h"
#include "restclient.h"


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

#if 0
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
#endif

  response->body = "GOOD";

  args->~RouteArguments();
  internal_free(args);

  return response;
}


Response *GallocyServer::route_request_vote(RouteArguments *args, Request *request) {
  gallocy::json request_json = request->get_json();
  uint64_t candidate_commit_index = request_json["commit_index"];
  uint64_t candidate_current_term = request_json["term"];
  uint64_t candidate_last_applied = request_json["last_applied"];
  uint64_t candidate_voted_for = request->peer_ip;
  uint64_t local_commit_index = gallocy_state->get_commit_index();
  uint64_t local_current_term = gallocy_state->get_current_term();
  uint64_t local_last_applied = gallocy_state->get_last_applied();
  uint64_t local_voted_for = gallocy_state->get_voted_for();
  bool granted = false;

  if (candidate_current_term < local_current_term) {
    granted = false;
  } else if (local_voted_for == 0
      || local_voted_for == candidate_voted_for) {
    if (candidate_last_applied >= local_last_applied
        && candidate_commit_index >= local_commit_index) {

      LOG_INFO("Granting vote to "
          << utils::unparse_internet_address(candidate_voted_for)
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
  // TODO(sholsapp): This should actually be a gallocy::vector<Command> (from
  // the consensus log module).
  gallocy::vector<gallocy::string> leader_entries = request_json["entries"];
  uint64_t leader_commit_index = request_json["leader_commit"];
  uint64_t leader_prev_log_index = request_json["previous_log_index"];
  uint64_t leader_prev_log_term = request_json["previous_log_term"];
  uint64_t leader_term = request_json["term"];
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
    // if (leader_entries.size() > 0) {
    //   LOG_INFO("Appending " << leader_entries.size() << " new entries!");
    // }
    success = true;
    gallocy_state->set_current_term(leader_term);
    gallocy_state->set_state(RaftState::FOLLOWER);
    gallocy_state->set_voted_for(request->peer_ip);
    gallocy_state->get_timer()->reset();
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


// TODO(sholsapp): clean this up after we're done testing.
Response *GallocyServer::route_request(RouteArguments *args, Request *request) {

  // TODO(sholsapp): Factor this code into a GallocyRequestClient that can be
  // used everywhere.
  uint64_t leader_term = gallocy_state->get_current_term();
  uint64_t leader_last_applied = gallocy_state->get_last_applied();
  uint64_t leader_commit_index = gallocy_state->get_commit_index();
  uint64_t leader_prev_log_index = gallocy_state->get_log()->get_previous_log_index();
  uint64_t leader_prev_log_term = gallocy_state->get_log()->get_previous_log_term();

  std::function<bool(const RestClient::Response &)> callback = [](const RestClient::Response &rsp) {
    if (rsp.code == 200) {
      gallocy::string body = rsp.body.c_str();
      gallocy::json response_json = gallocy::json::parse(body);
      bool success = response_json["success"];
      uint64_t supporter_term = response_json["term"];
      uint64_t local_term = gallocy_state->get_current_term();
      if (supporter_term > local_term) {
        gallocy_state->set_state(RaftState::FOLLOWER);
        gallocy_state->set_current_term(supporter_term);
      }
      return success;
    }
    return false;
  };

  gallocy::json j = {
    // TODO(sholsapp): What does a Command look like as JSON?
    { "entries", { "hello world" } },
    { "leader_commit", leader_commit_index },
    { "previous_log_index", leader_prev_log_index },
    { "previous_log_term", leader_prev_log_term },
    { "term", leader_term },
  };

  // TODO(sholsapp): How we handle this is busted and needs to be refactored so
  // that the cv is usable here. This is also blocking, which is probably bad?
  uint64_t votes = utils::post_many("/raft/append_entries", config.peers, config.port, j.dump(), callback);

  Response *response = new (internal_malloc(sizeof(Response))) Response();
  response->headers["Server"] = "Gallocy-Httpd";
  response->headers["Content-Type"] = "application/json";
  response->status_code = 200;
  response->body = "GOOD";
  args->~RouteArguments();
  internal_free(args);
  return response;
}


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
