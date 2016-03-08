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
