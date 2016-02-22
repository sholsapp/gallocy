#include "gtest/gtest.h"

#include <stdio.h>
#include "gallocy/http/client.h"

gallocy::string UDPCLIENT_GET_REQUEST(
   "GET /test/uri HTTP/1.1\r\n"
   "Host: 127.0.0.1:12345\r\n"
   "\r\n"
);

gallocy::string UDPCLIENT_POST_REQUEST(
   "POST /data/goes/here HTTP/1.1\r\n"
   "Host: 127.0.0.1:12345\r\n"
   "Content-Type: application/json\r\n"
   "Content-Length: 14\r\n"
   "\r\n"
   "Best json ever"
);

TEST(UDPClientTests, GetRequest) {
  uint16_t server_port = 32345;
  gallocy::common::Peer peer("127.0.0.1", server_port);
  gallocy::http::Request get(UDPCLIENT_GET_REQUEST);
  get.peer = peer;
  gallocy::http::UDPClient udp_client;
  
  // For now the udp transport class will act as a server
  gallocy::http::UDPTransport udp(peer, server_port);

  udp_client.request(get);
  gallocy::string http = udp.read();

  // UDPTransport will not respond should have 3x the get request
  // TODO(rverdon): How to handle this case on the server, What to do with multiple repeated requests
  ASSERT_EQ(http.length()/3, UDPCLIENT_GET_REQUEST.length());
}

