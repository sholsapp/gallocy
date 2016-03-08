#include <arpa/inet.h>
#include <netinet/in.h>

#include "gtest/gtest.h"

#include "gallocy/common/peer.h"


TEST(PeerTests, Constructors) {
    gallocy::common::Peer peer1;
    ASSERT_EQ(peer1.get_canonical_id(), static_cast<uint64_t>(0));
    ASSERT_EQ(peer1.get_string(), "0.0.0.0:0");
    gallocy::common::Peer peer2("127.0.0.1", 8080);
    ASSERT_EQ(peer2.get_canonical_id(), static_cast<uint64_t>(2130706433));
    ASSERT_EQ(peer2.get_string(), "127.0.0.1:8080");
    gallocy::common::Peer peer3 = peer2;
    ASSERT_EQ(peer3.get_canonical_id(), static_cast<uint64_t>(2130706433));
    ASSERT_EQ(peer3.get_string(), "127.0.0.1:8080");
    gallocy::common::Peer peer4(peer2);
    ASSERT_EQ(peer4.get_canonical_id(), static_cast<uint64_t>(2130706433));
    ASSERT_EQ(peer4.get_string(), "127.0.0.1:8080");
}


TEST(PeerTests, GetSocket) {
    gallocy::common::Peer peer1("127.0.0.1", 8080);
    ASSERT_EQ(peer1.get_string(), "127.0.0.1:8080");
    ASSERT_EQ(ntohs(peer1.get_socket().sin_port), 8080);
}


TEST(PeerTests, FromSocket) {
    struct sockaddr_in socket_name;
    socket_name.sin_family = AF_INET;
    socket_name.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &socket_name.sin_addr);
    gallocy::common::Peer peer1(socket_name);
    ASSERT_EQ(peer1.get_string(), "127.0.0.1:8080");
}
