#include "config.h"

#include <string>

#include "external/json.hpp"
#include "gallocy/stringutils.h"


gallocy::json load_config(const gallocy::string &path) {
  return gallocy::json::parse(
      utils::read_file(path.c_str()).c_str());
}
