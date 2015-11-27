#ifndef GALLOCY_HEAPLAYERS_SOURCE_H_
#define GALLOCY_HEAPLAYERS_SOURCE_H_

#include <sys/mman.h>

#include "heaplayers/myhashmap.h"
#include "heaplayers/spinlock.h"
#include "heaplayers/lockedheap.h"
#include "heaplayers/freelistheap.h"
#include "heaplayers/firstfitheap.h"
#include "heaplayers/zoneheap.h"
#include "heaplayers/sizeheap.h"
#include "heaplayers/heaptypes.h"

#include "gallocy/constants.h"


#define ZONE_SZ   4096 * 4096 * 16
#define MMAP_PROT PROT_READ|PROT_WRITE
#define MMAP_FLAG MAP_ANON|MAP_SHARED


namespace HL {


class SourceMmapHeap {
 public:
  inline void* malloc(size_t sz) {
    void* mem = NULL;
    if (!zone) {
      if ((zone = mmap(reinterpret_cast<void *>(global_base()), ZONE_SZ, MMAP_PROT, MMAP_FLAG, -1, 0)) == MAP_FAILED) {
        fprintf(stderr, "---ENONMEM---\n");
        abort();
      }
      next = reinterpret_cast<char*>(zone);
      bytes_left = ZONE_SZ;
    }
    if (bytes_left > sz) {
      mem  = reinterpret_cast<void*>(next);
      next = next + sz;
      bytes_left -= sz;
      return mem;
    } else {
      fprintf(stderr, "---ENONMEM---\n");
      abort();
    }
  }

  inline void free(void* ptr) {
    // TODO(sholsapp): This needs to be re-added or cleaned up.
    // munmap(reinterpret_cast<char*>(ptr), getSize(ptr));
    return;
  }

  inline void free(void* ptr, size_t sz) {
    // TODO(sholsapp): This needs to be re-added or cleaned up.
    // munmap(reinterpret_cast<char*>(ptr), getSize(ptr));
    return;
  }

  inline size_t getSize(void* ptr) {
    return -1;
  }

  inline void __reset() {
    zone = NULL;
    next = NULL;
    bytes_left = 0;
  }

 private:
  void *zone;
  char *next;
  uint64_t bytes_left;
};


class SingletonHeap {
 public:
  static SingletonHeap& getInstance() {
    static SingletonHeap instance;
    return instance;
  }

  static void* malloc(size_t sz) {
    return heap.malloc(sz);
  }

  static void free(void* ptr) {
    heap.free(ptr);
  }

  static void __reset() {
    heap.__reset();
  }

 private:
  static SingletonHeapType heap;
  // Need public for STL allocators.
  // SingletonHeap() {};
  // SingletonHeap(SingletonHeap const&);
  // void operator=(SingletonHeap const&);
};

}  // namespace HL

#endif  // GALLOCY_HEAPLAYERS_SOURCE_H_