#ifndef GALLOCY_UTILS_CONFIG_H_
#define GALLOCY_UTILS_CONFIG_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "gallocy/libgallocy.h"


/**
 * The gallocy configuration.
 *
 * As of today, exists soley to hide the ugly fucking handling of
 * nlohmann::json implicit converstion issues from the rest of the application.
 */
class GallocyConfig {
 public:
  GallocyConfig(gallocy::string &address, gallocy::vector<gallocy::string> &peers, uint16_t port)
    : address(address),
      peers(peers),
      port(port)
  {}
  explicit GallocyConfig(gallocy::json j) {
    port = j["port"];

    // TODO(sholsapp): Gah, this is driving me insane, we need to fix this
    // implicit converstion nightmare.
    gallocy::json::string_t _address = j["self"];
    address = _address.c_str();

    // TODO(sholsapp): Gah, this is driving me insane, we need to fix this
    // implicit converstion nightmare.
    gallocy::json::array_t _peers = j["peers"];
    for (auto p : _peers) {
      gallocy::json::string_t str = p;
      gallocy::string _p = str.c_str();
      peers.push_back(_p);
    }
  }

 public:
  bool master;
  gallocy::string address;
  gallocy::vector<gallocy::string> peers;
  uint16_t port;
};


GallocyConfig *load_config(const gallocy::string &);

#endif  // GALLOCY_UTILS_CONFIG_H_
