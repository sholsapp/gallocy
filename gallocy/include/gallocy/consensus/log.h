#ifndef GALLOCY_CONSENSUS_LOG_H_
#define GALLOCY_CONSENSUS_LOG_H_

#include <vector>

#include "gallocy/allocators/internal.h"

/**
 * A replicated state machine command.
 *
 * A command is the most granular unit that can be applied to the replicated
 * state machine.
 */
class Command {
 public:
  explicit Command(gallocy::string command) :
    command(command) {}
  // Command(const Command &) = delete;
  // Command &operator=(const Command &) = delete;

 public:
  gallocy::string command;
};


/**
 * A replicated state machine log entry.
 *
 * A log entry is simply a bundle of :class:`Command` and the term when the
 * entry was received by the leader.
 */
class LogEntry {
 public:
  LogEntry(Command command, uint64_t term) :
    command(command),
    committed(false),
    term(term) {}
  LogEntry(const LogEntry &) = delete;
  LogEntry &operator=(const LogEntry &) = delete;
  gallocy::json to_json() const {
    gallocy::json j = {
      { "term", term },
      { "command", command.command }
    };
    return j;
  }

 public:
  Command command;
  bool committed;
  uint64_t term;
};


/**
 * A replicated state machine log.
 *
 * A log is the primary data structure that we use to build a replicated state
 * machine. A log is a consistent data structure across all peers that
 * represents a sequence of commands.
 */
class GallocyLog {
 public:
  GallocyLog() {}
  GallocyLog(const GallocyLog &) = delete;
  GallocyLog &operator=(const GallocyLog &) = delete;
  /**
   * Get the index of the last committed log entry.
   *
   * Gets the index of log entry immediately preceding any uncommitted log
   * entries, suitable for sending to peers during an append entries request.
   *
   * :returns: The index of the last committed log entry, or -1 if no committed
   * log entry exists.
   */
  int64_t get_previous_log_index() {
    for (int i = log.size() - 1; i >= 0; ++i) {
    if (!log[i].committed)
      return i;
    }
    return -1;
  }
  /**
   * Get the term of the last committed log entry.
   *
   * Gets the term of the log entry immediately preceding any uncommitted log
   * entries, suitable for sending to peers during an append entries request.
   *
   * :returns: The term of the last committed log entry, or -1 if no committed
   * log entry exists.
   */
  int64_t get_previous_log_term() {
    for (int i = log.size() - 1; i >= 0; ++i) {
    if (!log[i].committed)
      return log[i].term;
    }
    return -1;
  }

 public:
  gallocy::vector<LogEntry> log;
};

#endif  // GALLOCY_CONSENSUS_LOG_H_
