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

#define LOG_APP(message) { \
  gallocy::stringstream s; \
  s << message; \
  __log(__FILE__, "APP", s.str().c_str()); \
}

//
// Maintainers, see http://misc.flogisoft.com/bash/tip_colors_and_formatting
// for a list of color codes that you can use if what you want is missing.
//

#define L_RED(message) \
  "\033[0;31m" << message << "\033[0m"

#define L_YELLOW(message) \
  "\033[0;33m" << message << "\033[0m"

#define L_GREEN(message) \
  "\033[0;32m" << message << "\033[0m"

#define L_BLUE(message) \
  "\033[0;34m" << message << "\033[0m"

#define L_GREY(message) \
  "\033[0;37m" << message << "\033[0m"

#define L_ORANGE(message) \
  "\033[38;5;208m" << message << "\033[0m"

void __log(const char *module, const char *level, const char *raw_message);

#endif  // GALLOCY_LOGGING_H_
