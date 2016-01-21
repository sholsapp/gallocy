#ifndef GALLOCY_CONSENSUS_LOG_H_
#define GALLOCY_CONSENSUS_LOG_H_

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
    command(command) {};
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
    term(term) {}
  LogEntry(const LogEntry &) = delete;
  LogEntry &operator=(const LogEntry &) = delete;
 public:
  Command command;
  uint64_t term;
};


/**
 * A replicated state machine log.
 *
 * A log is the primary data structure that we use to build a replicated state
 * machine. A log is a consistent datastructure accross all peers that
 * represents a sequence of commands.
 */
class GallocyLog {
 public:
  GallocyLog() {}
  GallocyLog(const GallocyLog &) = delete;
  GallocyLog &operator=(const GallocyLog &) = delete;
 public:
  gallocy::vector<LogEntry> log;
};

#endif  // GALLOCY_CONSENSUS_LOG_H_
