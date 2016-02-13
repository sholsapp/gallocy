
#include <cstdio>
#include <cstring>
#include <string>

#include "gallocy/allocators/shared.h"
#include "gallocy/sqlite.h"
#include "gallocy/utils/logging.h"


sqlite3_mem_methods my_mem = {
  &sqlite_malloc,
  &sqlite_free,
  &sqlite_realloc,
  &sqlite_size,
  &sqlite_roundup,
  &sqlite_init,
  &sqlite_shutdown,
  0
};


void init_sqlite_memory() {
  if (sqlite3_config(SQLITE_CONFIG_MALLOC, &my_mem) != SQLITE_OK) {
    LOG_ERROR("Failed to set custom sqlite memory allocator!");
    abort();
  }
  return;
}

