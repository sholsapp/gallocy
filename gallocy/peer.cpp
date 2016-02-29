#include <arpa/inet.h>
#include <netinet/in.h>

#include "gallocy/allocators/internal.h"
#include "gallocy/peer.h"


const gallocy::string gallocy::common::Peer::get_string() const {
  gallocy::stringstream me;
  me << unparse_internet_address(internet_address_integer)
     << ":" << port_integer;
  return me.str();
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


const struct sockaddr_in gallocy::common::Peer::get_socket() const {
  struct sockaddr_in ip_address;
  memset(std::addressof(ip_address), 0, sizeof(struct sockaddr_in));
  ip_address.sin_family = AF_INET;
  ip_address.sin_port = htons(port_integer);
  ip_address.sin_addr.s_addr = htonl(internet_address_integer & 0xFFFFFFFF);
  return ip_address;
}


const uint16_t gallocy::common::Peer::get_port() const {
  return port_integer;
}


const gallocy::string gallocy::common::Peer::unparse_internet_address(uint64_t ip_address) const {
  // HACK off half of the uint64_t representation after converting it to
  // network order.
  uint32_t half_of_long = static_cast<uint32_t>(htonl(ip_address));
  struct in_addr ip_addr;
  ip_addr.s_addr = half_of_long;
  return inet_ntoa(ip_addr);
}


bool gallocy::common::Peer::operator== (const gallocy::common::Peer &rhs) const {
  return get_canonical_id() == rhs.get_canonical_id() && get_port() == rhs.get_port();
}


bool gallocy::common::Peer::operator!= (const gallocy::common::Peer &rhs) const {
  return !(*this == rhs);
}
