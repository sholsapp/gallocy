#include <arpa/inet.h>

#include "gallocy/allocators/internal.h"
#include "gallocy/peer.h"


const gallocy::string gallocy::common::Peer::get_string() const {
  return unparse_internet_address(internet_address_integer);
}


const uint64_t gallocy::common::Peer::get_canonical_id() const {
  // TODO(sholsapp): When we wish to run two peers on the same machine, we'll
  // need to adjust this so that we include the port as well.
  return internet_address_integer;
}


const uint64_t gallocy::common::Peer::parse_internet_address(const gallocy::string &ip_address) const {
  uint64_t ip = 0;
  uint8_t buf[4];
  int r = inet_pton(AF_INET, ip_address.c_str(), buf);
  if (r <= 0) {
    return 0;
  }
  for (int i = 0; i < 4; i++) {
    ip <<= 8;
    ip |= buf[i] & 0xff;
  }
  return ip;
}


const gallocy::string gallocy::common::Peer::unparse_internet_address(uint64_t ip_address) const {
  // HACK off half of the uint64_t representation after converting it to
  // network order.
  uint32_t half_of_long = static_cast<uint32_t>(htonl(ip_address));
  struct in_addr ip_addr;
  ip_addr.s_addr = half_of_long;
  return inet_ntoa(ip_addr);
}
