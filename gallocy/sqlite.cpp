#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <utility>

#include "gallocy/libgallocy.h"
#include "gallocy/sqlite.h"
#include "heaplayers/heaptypes.h"


class xSizeHeap: public SqlitexSizeHeapType {};


typedef
  std::map<void*, int,
  std::less<void*>,
  STLAllocator<std::pair<void*, int>, xSizeHeap> >
    xSizeMapType;


/**
 * This is a "page table" helper allocator that also is needed to coordinate
 * global application memory. We keep it seperate from the sqlite allocator
 * because of weird things the STL library does with memory.
 */
xSizeMapType xSizeMap;


class SqliteHeap: public SqliteAllocatorHeapType {};

/**
 * This is the "page table" used to coodinate global application memory between
 * nodes.
 */
SqliteHeap sqliteHeap;


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
    std::cout << ptr << " " << min_size << std::endl;
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


sqlite3_mem_methods my_mem = {
  &xMalloc,
  &xFree,
  &xRealloc,
  &xSize,
  &xRoundup,
  &xInit,
  &xShutdown,
  0
};


void init_sqlite_memory() {
  if (sqlite3_config(SQLITE_CONFIG_MALLOC, &my_mem) != SQLITE_OK) {
    fprintf(stderr, "Failed to set custom sqlite memory allocator!\n");
    return;
  }
  fprintf(stderr, "Successfully set custom sqlite memory allocator!\n");
  return;
}

