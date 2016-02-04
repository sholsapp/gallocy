#ifndef GALLOCY_CONSENSUS_CLIENT_H_
#define GALLOCY_CONSENSUS_CLIENT_H_

#include "gallocy/utils/config.h"

class GallocyClient {
 public:
  explicit GallocyClient(GallocyConfig &config)
    : config(config)
  {}
  /**
   * Make a "request vote" request.
   *
   * This method makes many requests internally, which is required to make a
   * single "request vote" request. This is the nature of consensus.
   *
   * \return True if the success criteria is met.
   */
  bool send_request_vote();
  /**
   * Make a "append entries" request.
   *
   * This method makes many requests internally, which is required to make a
   * single "append entries" request. This is the nature of consensus.
   *
   * \return True if the success criteria is met.
   */
  bool send_append_entries();

 private:
  GallocyConfig &config;
};

#endif  // GALLOCY_CONSENSUS_CLIENT_H_
