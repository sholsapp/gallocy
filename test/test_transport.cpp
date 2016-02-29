#include "gtest/gtest.h"

#include "gallocy/http/transport.h"
#include "gallocy/utils/stringutils.h"


TEST(UDPTransportTests, Constructor) {
  uint64_t dest_addr = utils::parse_internet_address("127.0.0.1");
  uint16_t dest_port = 12345;
  gallocy::http::UDPTransport udp(dest_addr, dest_port, 0);
  ASSERT_NE(udp.sock, 0);
}


TEST(UDPTransportTests, SendReceive) {
  uint64_t dest_addr = utils::parse_internet_address("127.0.0.1");
  uint16_t port1 = 12346;
  uint16_t port2 = 64321;
  gallocy::http::UDPTransport udp1(dest_addr, port2, port1);
  gallocy::http::UDPTransport udp2(dest_addr, port1, port2);
  gallocy::string test_data = "Best HTTP ever";

  udp1.write(test_data);
  gallocy::string data = udp2.read();
  ASSERT_EQ(test_data, data);
}


TEST(UDPTransportTests, BigSendReceive) {
  uint64_t dest_addr = utils::parse_internet_address("127.0.0.1");
  uint16_t port1 = 12355;
  uint16_t port2 = 55321;
  gallocy::http::UDPTransport udp1(dest_addr, port2, port1);
  gallocy::http::UDPTransport udp2(dest_addr, port1, port2);
  gallocy::string test_data(6000, 'a');

  udp1.write(test_data);
  gallocy::string data = udp2.read();
  ASSERT_EQ(test_data.length(), data.length());
  ASSERT_EQ(test_data, data);
}
