#include "gallocy/consensus/log.h"


int64_t GallocyLog::get_previous_log_index() {
    for (int i = log.size() - 1; i >= 0; ++i) {
        if (!log[i].committed)
            return i;
    }
    return 0;
}


int64_t GallocyLog::get_previous_log_term() {
    for (int i = log.size() - 1; i >= 0; ++i) {
        if (!log[i].committed)
            return log[i].term;
    }
    return 0;
}


int64_t GallocyLog::append_entry(const LogEntry &entry) {
    log.push_back(entry);
    return log.size() - 1;
}

