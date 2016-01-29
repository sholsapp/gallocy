#include <time.h>

#include <iostream>
#include <vector>

#include "gallocy/allocators/internal.h"
//#include "gallocy/glibc/time.h"
#include "gallocy/logging.h"
#include "gallocy/stringutils.h"


gallocy::string level_string(const char *level) {
  gallocy::string log_level = level;
  gallocy::stringstream s;
  if (log_level.compare("DEBUG") == 0) {
    s << L_GREY("[" << level << "]");
  } else if (log_level.compare("INFO") == 0) {
    s << L_GREEN("[" << level << "]");
  } else if (log_level.compare("WARNING") == 0) {
    s << L_YELLOW("[" << level << "]");
  } else if (log_level.compare("ERROR") == 0) {
    s << L_RED("[" << level << "]");
  } else if (log_level.compare("APP") == 0) {
    s << L_BLUE("[" << level << "]");
  } else {
    s << level;
  }
  return s.str();
}


void __log(const char *module, const char *level, const char *raw_message) {
  gallocy::vector<gallocy::string> path_parts;
  gallocy::string message = raw_message;
  utils::split(module, '/', path_parts);

  time_t t = std::time(0);
  struct tm now = {0};
  gmtime_r(&t, &now);
  // See gmtime_r(3)
  char utc_now[256] = {0};
  asctime_r(&now, utc_now);
  gallocy::string _utc_now = utc_now;

  gallocy::stringstream line;
  line << level_string(level).c_str()
       << " - "
       << utils::trim(_utc_now)
       << " - "
       << utils::trim(path_parts.at(path_parts.size() - 1))
       << " - "
       << utils::trim(message)
       << std::endl;
  fprintf(stderr, "%s", line.str().c_str());
}
