#include "gtest/gtest.h"

#include "gallocy/http/transport.h"


TEST(UDPTransportTests, Constructor) {
  gallocy::common::Peer peer("127.0.0.1", 12345);
  
  gallocy::http::UDPTransport udp(peer, 0);
  ASSERT_NE(udp.sock, 0);
}


TEST(UDPTransportTests, SendReceive) {
  uint16_t port1 = 12345;
  uint16_t port2 = 54321;
  gallocy::common::Peer peer1("127.0.0.1", port1);
  gallocy::common::Peer peer2("127.0.0.1", port2);
    
  gallocy::http::UDPTransport udp1(peer1, port2);
  gallocy::http::UDPTransport udp2(peer2, port1);
  gallocy::string test_data = "Best HTTP ever";

  udp1.write(test_data);
  gallocy::string data = udp2.read();
  ASSERT_EQ(test_data, data);
}


TEST(UDPTransportTests, BigSendReceive) {
  uint16_t port1 = 12345;
  uint16_t port2 = 54321;
  gallocy::common::Peer peer1("127.0.0.1", port1);
  gallocy::common::Peer peer2("127.0.0.1", port2);
    
  gallocy::http::UDPTransport udp1(peer1, port2);
  gallocy::http::UDPTransport udp2(peer2, port1);
  gallocy::string test_data(6000, 'a');

  udp1.write(test_data);
  gallocy::string data = udp2.read();
  ASSERT_EQ(test_data.length(), data.length());
  ASSERT_EQ(test_data, data);
}
