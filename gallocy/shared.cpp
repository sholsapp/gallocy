#include <stdint.h>

#include <cstdlib>
#include <map>

#include "allocators/shared.h"
#include "heaplayers/stl.h"

HL::SingletonSharedHeapType shared_page_table_heap;
HL::SingletonSharedHeapType HL::SharedPageTableHeap::heap = shared_page_table_heap;
HL::SharedPageTableHeap shared_page_table;


// TODO(sholsapp): This module needs some names cleaning up to match style
// with the rest of the modules.

typedef
  std::map<void *, int,
  std::less<void *>,
  STLAllocator<std::pair<void *, int>, HL::SharedPageTableHeap> >
    xSizeMapType2;

xSizeMapType2 xSizeMap;


void* xMalloc(int sz) {
  if (sz == 0) {
    return NULL;
  }
  void* ptr = shared_page_table.malloc(sz);
  xSizeMap.insert(std::pair<void*, int>(ptr, sz));
  return ptr;
}


void xFree(void* ptr) {
  xSizeMap.erase(ptr);
  shared_page_table.free(ptr);
  return;
}


void* xRealloc(void* ptr, int sz) {
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


int xSize(void* ptr) {
  int sz = xSizeMap[ptr];
  return sz;
}


int xRoundup(int sz) {
  return sz;
}


int xInit(void* ptr) {
  fprintf(stderr, "xInit\n");
  return 0;
}


void xShutdown(void* ptr) {
  fprintf(stderr, "xShutdown\n");
  return;
}
