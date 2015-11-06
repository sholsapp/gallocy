#include "config.h"

#include <string>

#include "./stringutils.h"
#include "external/json.hpp"


gallocy::json load_config(const gallocy::string &path) {
  return gallocy::json::parse(
      utils::read_file(path.c_str()).c_str());
}
