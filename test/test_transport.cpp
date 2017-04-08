#include "gtest/gtest.h"

#include "gallocy/http/transport.h"


int tcp_sock = -1;
uint16_t tcp_port = 22222;

class TCPTransportTests: public ::testing::Test {
 protected:
  virtual void SetUp() {
    if (tcp_sock == -1) {
      if ((tcp_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("test_transport socket");
        exit(1);
      }

      struct sockaddr_in listen_addr;
      memset(std::addressof(listen_addr), 0, sizeof(listen_addr));
      listen_addr.sin_family = AF_INET;
      listen_addr.sin_port = htons(tcp_port);
      listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);

      if (bind(tcp_sock, reinterpret_cast<struct sockaddr*>(std::addressof(listen_addr)), sizeof(listen_addr)) < 0) {
        perror("test_transport bind");
        exit(1);
      }

      if (listen(tcp_sock, 5) < 0) {
        perror("test_transport listen");
        exit(1);
      }
    }
  }

  virtual void TearDown() {
    close(tcp_sock); 
    tcp_sock = -1;
  }

};

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

TEST_F(TCPTransportTests, Constructor) {
  gallocy::common::Peer peer("127.0.0.1", tcp_port);
  
  gallocy::http::TCPTransport tcp(peer, 0);
  ASSERT_NE(tcp.sock, 0);
}

TEST_F(TCPTransportTests, SendReceive) {
  gallocy::common::Peer peer("127.0.0.1", tcp_port);
  char buf[6000] = {0};
  int read = 0;

  gallocy::http::TCPTransport tcp(peer, 0);
  gallocy::string test_data = "Best HTTP ever";

  tcp.write(test_data);

  int client_sock = accept(tcp_sock, NULL, NULL);
  read = recv(client_sock, buf, 6000, 0);
  
  gallocy::string data(buf, read);
  ASSERT_EQ(test_data.size(), read);
  ASSERT_EQ(test_data, data);
  close(client_sock);
  tcp_port++;
}

TEST_F(TCPTransportTests, BigSendReceive) {
  gallocy::common::Peer peer("127.0.0.1", tcp_port);
  char buf[8000] = {0};
  int read = 0;
  gallocy::http::TCPTransport tcp(peer, 0);
  gallocy::string test_data(6000, 'a');

  tcp.write(test_data);
  
  int client_sock = accept(tcp_sock, NULL, NULL);
  read = recv(client_sock, buf, 8000, 0);
  
  gallocy::string data(buf, read);
  ASSERT_EQ(test_data.size(), read);
  ASSERT_EQ(test_data, data);
  close(client_sock);
  tcp_port++;
}
