#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "gtest/gtest.h"

#include "gallocy/allocators/internal.h"
#include "gallocy/consensus/server.h"
#include "gallocy/utils/config.h"
#include "gallocy/utils/http.h"
#include "restclient.h"


GallocyConfig *gallocy_config = nullptr;
GallocyServer *gallocy_server = nullptr;


class ConsensusServerTests: public ::testing::Test {
 protected:
  /**
   * Starts a GallocyServer.
   */
  virtual void SetUp() {
    gallocy::string address = "127.0.0.1";
    gallocy::vector<gallocy::string> peers;
    // Add oneself as a peer for testing routes.
    peers.push_back("http://127.0.0.1:8080");
    gallocy_config = new (internal_malloc(sizeof(GallocyConfig))) GallocyConfig(address, peers, 8080);
    gallocy_server = new (internal_malloc(sizeof(GallocyServer))) GallocyServer(*gallocy_config);
    gallocy_server->start();
    // TODO(sholsapp): Replace this with a "ready" implementation.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
  /**
   * Stops a GallocyServer.
   */
  virtual void TearDown() {
    // TODO(sholsapp): The server's join implementation blocks forever
    // because, although we've set alive = false, it is blocked indefinitly
    // waiting for new connections. This approach joins the server in a thread
    // and then executes one last request to prompt the server to actually shut
    // down. Fix by using select in the server with a timeout.
    std::thread([&]{ gallocy_server->stop(); }).detach();
    RestClient::Response rsp = RestClient::get("http://127.0.0.1:8080/admin");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Clean up
    gallocy_config->~GallocyConfig();
    internal_free(gallocy_config);
    gallocy_server->~GallocyServer();
    internal_free(gallocy_server);
  }
};


TEST_F(ConsensusServerTests, StartStop) {
  uint64_t rsp = utils::post_many("/admin", gallocy_config->peers, 8080, "",
      [](const RestClient::Response &rsp) {
        return true;
      });
  ASSERT_GE(rsp, 0);
}
