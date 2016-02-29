#ifndef GALLOCY_CONSENSUS_CLIENT_H_
#define GALLOCY_CONSENSUS_CLIENT_H_

#include <vector>

#include "gallocy/consensus/log.h"
#include "gallocy/utils/config.h"

namespace gallocy {

namespace consensus {

/**
 * The GallocyClient object.
 *
 * The GallocyClient, or client, is used by a candidate or leader to change the
 * distributed state machine in some way. For example, a candidate may use the
 * client to start an election, or a leader may use the client to append
 * entries.
 *
 * The client is not used directly by a follower, since a follower only applies
 * changes from a leader. For logic pertaining to a follower, see \ref
 * GallocyServer.
 */
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

}  // namespace consensus

}  // namespace gallocy

#endif  // GALLOCY_CONSENSUS_CLIENT_H_
