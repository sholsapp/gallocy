#ifndef GALLOCY_PEER_H_
#define GALLOCY_PEER_H_

#include <arpa/inet.h>
#include <netinet/in.h>

#include <functional>

#include "gallocy/allocators/internal.h"

namespace gallocy {

namespace common {

/**
 * A peer abstraction.
 *
 * This abstraction should normalize how we deal with peers everywhere in the
 * code. It is a common code that should be used within the consensus and http
 * modules, but should not maintain state specific to specialized use cases.
 */
class Peer {
 public:
  /**
   * Create a peer.
   */
  Peer() : internet_address_integer(0), port_integer(0) {}
  /**
   * Create a peer.
   *
   * \param client_name The client socket name.
   */
  explicit Peer(struct sockaddr_in client_name) {
    internet_address_integer = parse_internet_address(inet_ntoa(client_name.sin_addr));
    port_integer = static_cast<uint16_t>(ntohs(client_name.sin_port));
  }
  /**
   * Create a peer.
   *
   * \param internet_address The dot notation internet address string.
   * \param port The port.
   */
  Peer(const gallocy::string &internet_address, uint16_t port) {
    internet_address_integer = parse_internet_address(internet_address);
    port_integer = port;
  }
  /**
   * Create a peer.
   *
   * \param internet_address The integer representation of the internet address.
   * \param port The port.
   */
  Peer(uint64_t internet_address, uint16_t port) {
    internet_address_integer = internet_address;
    port = port;
  }
  /**
   * Get the peer's internet address as a string in dot notation.
   *
   * \warning Always prefer \ref Peer::get_canonical_id when implementing
   * internal logic that depends on a unique identifier for the peer.
   *
   * \return The internet address string.
   */
  const gallocy::string get_string() const;
  /**
   * Get the peer's canonical identifier.
   *
   * \return The peer's canonical identifier.
   */
  const uint64_t get_canonical_id() const;

 private:
  /**
   * Unparse an IPv4 internet address from an integer.
   *
   *  \warning This function, like its counterpart, naively unparses an IPv4
   *  internet address into a string. This function needs to be rewritten such
   *  that endianness and IPv6 is handled.
   *
   * \param The IPv4 internet address as an integer.
   * \return A dot-notation string of the internet address.
   */
  const gallocy::string unparse_internet_address(uint64_t ip_address) const;
  /**
   * Parse an IPv4 internet address from a string.
   *
   *  \warning This function naively parses an IPv4 internet address from a
   *  string.  This function needs to be rewritten such that endian-ness and
   *  IPv6 is handled.
   *
   * \param ip_address The IPv4 internet address as a string.
   * \return An unsigned 64 bit integer of the IPv4 address.
   */
  const uint64_t parse_internet_address(const gallocy::string &ip_address) const;
  /**
   * Used as the peer's canonical identifier.
   */
  uint64_t internet_address_integer;
  /**
   * The port.
   */
  uint16_t port_integer;
};

}  // namespace common

}  // namespace gallocy


namespace std {

// COMPLETE specialization allows the Peer type to be used as a key in map and
// set objects.
template<> struct less<gallocy::common::Peer> {
  bool operator() (const gallocy::common::Peer &lhs, const gallocy::common::Peer &rhs) const {
    return lhs.get_canonical_id() < rhs.get_canonical_id();
  }
};

}  // namespace std

#endif  // GALLOCY_PEER_H_
