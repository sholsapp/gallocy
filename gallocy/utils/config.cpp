#include <string>

#include "gallocy/allocators/internal.h"
#include "gallocy/config.h"
#include "gallocy/stringutils.h"


GallocyConfig *load_config(const gallocy::string &path) {
  return new (internal_malloc(sizeof(GallocyConfig)))
    GallocyConfig(gallocy::json::parse(utils::read_file(path.c_str()).c_str()));
}
