#include <stdint.h>

#include <cstdlib>
#include <map>

#include "allocators/shared.h"
#include "heaplayers/firstfitheap.h"
#include "heaplayers/lockedheap.h"
#include "heaplayers/sizeheap.h"
#include "heaplayers/source.h"
#include "heaplayers/spinlock.h"
#include "heaplayers/stdlibheap.h"
#include "heaplayers/stl.h"
#include "heaplayers/zoneheap.h"


HL::SingletonHeapType shared_page_table_heap;
HL::SingletonHeapType HL::SharedPageTableHeap::heap = shared_page_table_heap;
HL::SharedPageTableHeap shared_page_table;


typedef
  std::map<void *, int,
  std::less<void *>,
  STLAllocator<std::pair<void *, int>, HL::SharedPageTableHeap> >
    xSizeMapType2;


typedef
  HL::LockedHeap<
    HL::SpinLockType,
    HL::FirstFitHeap<
      HL::SizeHeap<
        HL::ZoneHeap<
          HL::SourceMmapHeap,
          DEFAULT_ZONE_SZ> > > >
  SqliteAllocatorHeapType2;


xSizeMapType2 xSizeMap;
SqliteAllocatorHeapType2 sqliteHeap;


void* xMalloc(int sz) {
  if (sz == 0) {
    return NULL;
  }
  void* ptr = sqliteHeap.malloc(sz);
  xSizeMap.insert(std::pair<void*, int>(ptr, sz));
  return ptr;
}


void xFree(void* ptr) {
  xSizeMap.erase(ptr);
  sqliteHeap.free(ptr);
  return;
}


void* xRealloc(void* ptr, int sz) {
  if (ptr == NULL) {
    return sqliteHeap.malloc(sz);
  }
  size_t min_size = sqliteHeap.getSize(ptr);
  void* buf = sqliteHeap.malloc(sz);
  if (buf != NULL) {
    memcpy(buf, ptr, min_size);
    sqliteHeap.free(ptr);
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
