#ifndef GALLOCY_STRINGUTILS_H_
#define GALLOCY_STRINGUTILS_H_

#include <vector>

#include "gallocy/libgallocy.h"


namespace utils {
  gallocy::string &ltrim(gallocy::string &);
  gallocy::string &rtrim(gallocy::string &);
  gallocy::string &trim(gallocy::string &);
  gallocy::string read_file(const char *);
  gallocy::vector<gallocy::string> &split(const gallocy::string &, char, gallocy::vector<gallocy::string> &);
  gallocy::string string_to_hex(const gallocy::string &input);
  bool startswith(gallocy::string hackstack, gallocy::string needle);
  bool endswith(gallocy::string hackstack, gallocy::string needle);
}

#endif  // GALLOCY_STRINGUTILS_H_
