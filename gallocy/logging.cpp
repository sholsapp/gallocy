#include <time.h>

#include <iostream>
#include <vector>

#include "gallocy/allocators/internal.h"
#include "gallocy/logging.h"
#include "gallocy/stringutils.h"


void __log(const char *module, const char *level, const char *raw_message) {
  gallocy::vector<gallocy::string> path_parts;
  gallocy::string message = raw_message;
  utils::split(module, '/', path_parts);

  time_t t = std::time(0);
  struct tm now = {0};
  gmtime_r(&t, &now);
  // See gmtime_r(3)
  char utc_now[256];
  asctime_r(&now, utc_now);

  gallocy::string _utc_now = utc_now;

  // TODO(sholsapp): Implement coloring using the codes at
  // http://www.tldp.org/HOWTO/Bash-Prompt-HOWTO/x329.html and add macros to
  // make this easier elsewhere.
  std::cout << "[" << "\033[0;33m" << level << "\033[0m" << "]"
            << " - "
            << utils::trim(_utc_now)
            << " - "
            << utils::trim(path_parts.at(path_parts.size() - 1))
            << " - "
            << utils::trim(message)
            << std::endl;
}
