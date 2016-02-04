#ifndef GALLOCY_CONSENSUS_CLIENT_H_
#define GALLOCY_CONSENSUS_CLIENT_H_

#include <vector>

#include "gallocy/consensus/log.h"
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
   * single "request vote" request.
   *
   * \return True if the success criteria is met.
   */
  bool send_request_vote();
  /**
   * Make an "append entries" request.
   *
   * This method is useful for creating "heart beat" messages which, per the
   * Raft protocol, is an "append entries" request with no entries. For
   * actually appending entries to the log, see \ref
   * GallocyClient::send_append_entries(const gallocy::vector<Command> &entries).
   *
   * This method makes many requests internally, which is required to make a
   * single "append entries" request.
   *
   * \return True if the success criteria is met.
   */
  bool send_append_entries();
  /**
   * Make an "append entries" request.
   *
   * This method is useful for creating messages which append entries to the
   * log and should not be used for "heart beat" messages. See \ref
   * GallocyClient::send_append_entries() for sending "heart beat" messages.
   *
   * This method makes many requests internally, which is required to make a
   * single "append entries" request.
   */
  bool send_append_entries(const gallocy::vector<LogEntry> &entries);

 private:
  GallocyConfig &config;
};

#endif  // GALLOCY_CONSENSUS_CLIENT_H_
