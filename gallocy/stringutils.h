#ifndef GALLOCY_STRINGUTILS_H_
#define GALLOCY_STRINGUTILS_H_

#include <vector>

#include "./libgallocy.h"


namespace utils {
  gallocy::string &ltrim(gallocy::string &);
  gallocy::string &rtrim(gallocy::string &);
  gallocy::string &trim(gallocy::string &);
  gallocy::vector<gallocy::string> &split(const gallocy::string &, char, gallocy::vector<gallocy::string> &);
}

#endif  // GALLOCY_STRINGUTILS_H_
