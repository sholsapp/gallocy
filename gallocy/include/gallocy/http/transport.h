#ifndef GALLOCY_HTTP_TRANSPORT_H_
#define GALLOCY_HTTP_TRANSPORT_H_

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

#include "gallocy/utils/stringutils.h"

#define UDP_TIMEOUT_100_MS 100000  // 100 Milliseconds
#define UDP_BUFSIZE 65507  // Largest IPV4 packet (65,535) - UDP header (8) - IPv4 Header(20)


namespace gallocy {

namespace http {

/**
 * An abstract transport layer.
 *
 * An abstract transport layer focuses on the protocol under HTTP,
 * which may be TCP, UDP, RDP, or something else. Higher level logic that
 * understands the specifics use of HTTP, e.g., a particular JSON API, should
 * be kept out of this layer. For logic of this type, see \ref GallocyClient.
 */
class AbstractTransport {
 public:
  /**
   * Read from the transport layer.
   *
   * \return HTTP data.
   */
  virtual gallocy::string read() = 0;
  /**
   * Write to the transport layer.
   *
   * \param http The HTTP to be written to the transport layer.
   */
  virtual void write(gallocy::string http) = 0;
};


/**
 * A request client that uses a raw TCP transport protocol.
 */
// TODO(rverdon): Implement me.
class TCPTransport: public AbstractTransport {
 public:
  /**
   * Read from the transport layer.
   *
   * \return HTTP data.
   */
  virtual gallocy::string read() = 0;
  /**
   * Write to the transport layer.
   *
   * \param http The HTTP to be written to the transport layer.
   */
  virtual void write(gallocy::string http) = 0;
};


/**
 * A request client that uses a raw UDP transport protocol.
 */
class UDPTransport: public AbstractTransport {
 public:
  /**
   * Read from the transport layer.
   *
   * \return HTTP data.
   */
  gallocy::string read();
  /**
   * Write to the transport layer.
   *
   * \param http The HTTP to be written to the transport layer.
   */
  void write(gallocy::string http);

  UDPTransport(uint64_t dest_addr, uint16_t dest_port, uint16_t listen_port);
  ~UDPTransport();

  int sock;
  // WHO to send data too
  struct sockaddr_in dst_addr;
  // LISTEN on this ip/port
  struct sockaddr_in listen_addr;
  // LAST ip data was received from
  struct sockaddr_in recv_addr;
  socklen_t recv_addr_len = sizeof(recv_addr);
  unsigned char buf[UDP_BUFSIZE];
};


/**
 * A request client that uses a raw RDP transport protocol.
 */
// TODO(rverdon): Implement me.
class RDPTransport: public AbstractTransport {
 public:
  /**
   * Read from the transport layer.
   *
   * \return HTTP data.
   */
  virtual gallocy::string read() = 0;
  /**
   * Write to the transport layer.
   *
   * \param http The HTTP to be written to the transport layer.
   */
  virtual void write(gallocy::string http) = 0;
};

}  // namespace http

}  // namespace gallocy

#endif  // GALLOCY_HTTP_TRANSPORT_H_
