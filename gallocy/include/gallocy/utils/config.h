#ifndef GALLOCY_UTILS_CONFIG_H_
#define GALLOCY_UTILS_CONFIG_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "gallocy/allocators/internal.h"
#include "gallocy/peer.h"


/**
 * The gallocy configuration.
 *
 * As of today, exists soley to hide the ugly fucking handling of
 * nlohmann::json implicit converstion issues from the rest of the application.
 */
class GallocyConfig {
 public:
  /**
   * Create a configuration.
   *
   * \param address This node's internet address.
   * \param peers This node's peer list.
   * \param port This node's port.
   */
  GallocyConfig(gallocy::string &address, gallocy::vector<gallocy::string> &peers, uint16_t port)
    : address(address),
      peers(peers),
      port(port) {}

  /**
   * Create a configuration.
   *
   * \param config_json A JSON object with keys for "self", "port", and "peers".
   */
  explicit GallocyConfig(gallocy::json config_json) {
    port = config_json["port"];

    // TODO(sholsapp): Gah, this is driving me insane, we need to fix this
    // implicit converstion nightmare.
    gallocy::json::string_t _address = config_json["self"];
    address = _address.c_str();

    for (auto p : config_json["peers"]) {
      gallocy::json::string_t str = p;
      gallocy::string _p = str.c_str();
      peers.push_back(_p);
    }
  }

 public:
  gallocy::string address;
  gallocy::vector<gallocy::string> peers;
  gallocy::vector<gallocy::common::Peer> peer_list;
  uint16_t port;
};


GallocyConfig *load_config(const gallocy::string &);

#endif  // GALLOCY_UTILS_CONFIG_H_
