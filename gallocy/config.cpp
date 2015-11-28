#include "gallocy/config.h"

#include <string>

#include "gallocy/stringutils.h"


GallocyConfig load_config(const gallocy::string &path) {
  return GallocyConfig(gallocy::json::parse(
      utils::read_file(path.c_str()).c_str()));
}
