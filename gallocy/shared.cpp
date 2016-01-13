#include <stdint.h>

#include <cstdlib>
#include <map>

#include "gallocy/allocators/shared.h"
#include "gallocy/heaplayers/stl.h"
#include "gallocy/logging.h"

HL::SingletonSharedHeapType shared_page_table_heap;
HL::SingletonSharedHeapType HL::SharedPageTableHeap::heap = shared_page_table_heap;
HL::SharedPageTableHeap shared_page_table;

typedef
  std::map<void *, int,
  std::less<void *>,
  STLAllocator<std::pair<void *, int>, HL::SharedPageTableHeap> >
    xSizeMapType2;

xSizeMapType2 xSizeMap;


void* sqlite_malloc(int sz) {
  if (sz == 0) {
    return NULL;
  }
  void* ptr = shared_page_table.malloc(sz);
  xSizeMap.insert(std::pair<void*, int>(ptr, sz));
  return ptr;
}


void sqlite_free(void* ptr) {
  xSizeMap.erase(ptr);
  shared_page_table.free(ptr);
  return;
}


void* sqlite_realloc(void* ptr, int sz) {
  if (ptr == NULL) {
    return shared_page_table.malloc(sz);
  }
  size_t min_size = shared_page_table.getSize(ptr);
  void* buf = shared_page_table.malloc(sz);
  if (buf != NULL) {
    memcpy(buf, ptr, min_size);
    shared_page_table.free(ptr);
  }
  xSizeMap.insert(std::pair<void*, int>(buf, sz));
  return buf;
}


int sqlite_size(void* ptr) {
  int sz = xSizeMap[ptr];
  return sz;
}


int sqlite_roundup(int sz) {
  return sz;
}


int sqlite_init(void* ptr) {
  LOG_DEBUG("Initialized SQLite database!");
  return 0;
}


void sqlite_shutdown(void* ptr) {
  LOG_DEBUG("Shutting down SQLite database!");
  return;
}
