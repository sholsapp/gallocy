#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "gtest/gtest.h"

#include "gallocy/allocators/internal.h"
#include "gallocy/consensus/client.h"
#include "gallocy/consensus/server.h"
#include "gallocy/consensus/state.h"
#include "gallocy/http/client.h"
#include "gallocy/http/request.h"
#include "gallocy/http/response.h"
#include "gallocy/peer.h"
#include "gallocy/utils/config.h"


GallocyClient *gallocy_client = nullptr;
GallocyConfig *gallocy_config = nullptr;
GallocyServer *gallocy_server = nullptr;
GallocyState *gallocy_state = nullptr;


class ConsensusServerTests: public ::testing::Test {
 protected:
  /**
   * Starts a GallocyServer.
   */
  virtual void SetUp() {
    if (gallocy_server != nullptr)
      return;
    gallocy::string address = "127.0.0.1";
    gallocy::vector<gallocy::common::Peer> peer_list;
    // Add oneself as a peer for testing routes.
    peer_list.push_back(gallocy::common::Peer("127.0.0.1", 8080));
    gallocy_state = new (internal_malloc(sizeof(GallocyState))) GallocyState();
    gallocy_config = new (internal_malloc(sizeof(GallocyConfig))) GallocyConfig(address, peer_list, 8080);
    gallocy_server = new (internal_malloc(sizeof(GallocyServer))) GallocyServer(*gallocy_config);
    gallocy_client = new (internal_malloc(sizeof(GallocyClient))) GallocyClient(*gallocy_config);
    gallocy_server->start();
    // TODO(sholsapp): Replace this with a "ready" implementation.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
  /**
   * Stops a GallocyServer.
   */
  // TODO(sholsapp): We need to fix up the destructors for the server before we
  // can clean up everything. Otherwise, when we try to recreate the server in
  // the fixture, the address is already in use.
  virtual void TearDown_Disabled() {
    // TODO(sholsapp): The server's join implementation blocks forever
    // because, although we've set alive = false, it is blocked indefinitly
    // waiting for new connections. This approach joins the server in a thread
    // and then executes one last request to prompt the server to actually shut
    // down. Fix by using select in the server with a timeout.
    std::thread([&]{ gallocy_server->stop(); }).detach();
    Response *rsp = CurlClient().request(Request("GET", gallocy::common::Peer("127.0.0.1", 8080), "/admin"));
    rsp->~Response();
    internal_free(rsp);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Clean up
    gallocy_client->~GallocyClient();
    gallocy_config->~GallocyConfig();
    gallocy_server->~GallocyServer();
    gallocy_state->~GallocyState();
    internal_free(gallocy_client);
    internal_free(gallocy_config);
    internal_free(gallocy_server);
    internal_free(gallocy_state);
    gallocy_client = nullptr;
    gallocy_config = nullptr;
    gallocy_server = nullptr;
    gallocy_state = nullptr;
  }
};


TEST_F(ConsensusServerTests, StartStop) {
  gallocy::vector<Request> requests;
  gallocy::map<gallocy::string, gallocy::string> headers;
  requests.push_back(Request("POST", gallocy::common::Peer("127.0.0.1", 8080), "/admin", "", headers));
  uint64_t rsp = CurlClient().multirequest(requests,
      [](const Response &rsp) {
        return true;
      }, nullptr, nullptr);
  ASSERT_GE(rsp, static_cast<uint64_t>(0));
}


TEST_F(ConsensusServerTests, RequestVote) {
  uint64_t votes = gallocy_client->send_request_vote();
  ASSERT_EQ(votes, static_cast<uint64_t>(1));
}


TEST_F(ConsensusServerTests, AppendEntries) {
  uint64_t successes = gallocy_client->send_append_entries();
  ASSERT_EQ(successes, static_cast<uint64_t>(1));
}