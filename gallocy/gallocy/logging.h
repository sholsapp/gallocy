#ifndef GALLOCY_LOGGING_H_
#define GALLOCY_LOGGING_H_

#include "gallocy/libgallocy.h"


#define LOG_INFO(message) { \
  gallocy::stringstream s; \
  s << message; \
  _log(__FILE__, "INFO", s.str().c_str()); \
}

void _log(const char *module, const char *level, const char *message);

#endif  // GALLOCY_LOGGING_H_
