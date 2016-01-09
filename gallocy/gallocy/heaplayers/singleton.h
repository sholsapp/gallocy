#ifndef GALLOCY_HEAPLAYERS_SINGLETON_H_
#define GALLOCY_HEAPLAYERS_SINGLETON_H_

#include "heaplayers/firstfitheap.h"
#include "heaplayers/sizeheap.h"
#include "heaplayers/source.h"
#include "heaplayers/zoneheap.h"
#include "heaplayers/stdlibheap.h"

// TODO(sholsapp): This is defined in several places.
#define DEFAULT_ZONE_SZ 16384 - 16

namespace HL {

typedef
  HL::StdlibHeap<
    HL::FirstFitHeap<
      HL::SizeHeap<
        HL::ZoneHeap<
          HL::SourceMmapHeap,
          DEFAULT_ZONE_SZ> > > >
  SingletonHeapType;

template <typename Heap>
class SingletonHeap {
 public:
  static SingletonHeap& getInstance() {
    static SingletonHeap instance;
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

  static void __reset() {
    heap.__reset();
  }

 private:
  static Heap heap;
  // Need public for STL allocators.
  // SingletonHeap() {};
  // SingletonHeap(SingletonHeap const&);
  // void operator=(SingletonHeap const&);
};

// Declare the static variable type but leave its specialization and definition
// to another compilation unit.
template <typename Heap>
Heap SingletonHeap<Heap>::heap;

}  // namespace HL

#endif  // GALLOCY_HEAPLAYERS_SINGLETON_H_

