#ifndef GALLOCY_UTILS_STRINGUTILS_H_
#define GALLOCY_UTILS_STRINGUTILS_H_

#include <vector>

#include "allocators/internal.h"


namespace utils {
  gallocy::string &ltrim(gallocy::string &);
  gallocy::string &rtrim(gallocy::string &);
  gallocy::string &trim(gallocy::string &);
  gallocy::string read_file(const char *);
  gallocy::vector<gallocy::string> &split(const gallocy::string &, char, gallocy::vector<gallocy::string> &);
  gallocy::string string_to_hex(const gallocy::string &input);
  bool startswith(gallocy::string hackstack, gallocy::string needle);
  bool endswith(gallocy::string hackstack, gallocy::string needle);
  uint64_t parse_internet_address(const gallocy::string &ip_address);
  const gallocy::string unparse_internet_address(uint64_t ip_address);
}  // namespace utils

#endif  // GALLOCY_UTILS_STRINGUTILS_H_
