#ifndef GALLOCY_LOGGING_H_
#define GALLOCY_LOGGING_H_

#include "allocators/internal.h"

#define LOG_DEBUG(message) { \
  gallocy::stringstream s; \
  s << message; \
  __log(__FILE__, "DEBUG", s.str().c_str()); \
}

#define LOG_INFO(message) { \
  gallocy::stringstream s; \
  s << message; \
  __log(__FILE__, "INFO", s.str().c_str()); \
}

#define LOG_WARNING(message) { \
  gallocy::stringstream s; \
  s << message; \
  __log(__FILE__, "WARNING", s.str().c_str()); \
}

#define LOG_ERROR(message) { \
  gallocy::stringstream s; \
  s << message; \
  __log(__FILE__, "ERROR", s.str().c_str()); \
}

void __log(const char *module, const char *level, const char *raw_message);

#endif  // GALLOCY_LOGGING_H_
