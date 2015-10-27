#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

#include "libgallocy.h"


namespace utils {

gallocy::string &ltrim(gallocy::string &);
gallocy::string &rtrim(gallocy::string &);
gallocy::string &trim(gallocy::string &);
gallocy::vector<gallocy::string> &split(const gallocy::string &, char, gallocy::vector<gallocy::string> &);
}
