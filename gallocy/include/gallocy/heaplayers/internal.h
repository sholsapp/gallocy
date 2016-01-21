#ifndef GALLOCY_HEAPLAYERS_INTERNAL_H_
#define GALLOCY_HEAPLAYERS_INTERNAL_H_

#include "heaplayers/firstfitheap.h"
#include "heaplayers/lockedheap.h"
#include "heaplayers/sizeheap.h"
#include "heaplayers/source.h"
#include "heaplayers/spinlock.h"
#include "heaplayers/stdlibheap.h"
#include "heaplayers/zoneheap.h"

// TODO(sholsapp): This is defined in several places.
#define DEFAULT_ZONE_SZ 16384 - 16

namespace HL {

typedef
  HL::StdlibHeap<
  HL::LockedHeap<
  HL::SpinLockType,
    HL::FirstFitHeap<
      HL::SizeHeap<
        HL::ZoneHeap<
          HL::SourceMmapHeap<PURPOSE_INTERNAL_HEAP>,
          DEFAULT_ZONE_SZ> > > > >
  SingletonInternalHeapType;

class InternalMemoryHeap {
 public:
  static InternalMemoryHeap& getInstance() {
    static InternalMemoryHeap instance;
    return instance;
  }

  static void* malloc(size_t sz) {
    return heap.malloc(sz);
  }

  static void* realloc(void *ptr, size_t sz) {
    return heap.realloc(ptr, sz);
  }

  char *strdup(const char *s1) {
    return heap.strdup(s1);
  }

  void *calloc(size_t count, size_t size) {
    return heap.calloc(count, size);
  }

  static void free(void* ptr) {
    heap.free(ptr);
  }

  inline size_t getSize(void *ptr) {
    return heap.getSize(ptr);
  }

  static void __reset() {
    heap.__reset();
  }

 private:
  static SingletonInternalHeapType heap;
  // Need public for STL allocators.
  // InternalMemoryHeap() {};
  // InternalMemoryHeap(InternalMemoryHeap const&);
  // void operator=(InternalMemoryHeap const&);
};

}  // namespace HL

#endif  // GALLOCY_HEAPLAYERS_INTERNAL_H_
