#ifndef GALLOCY_HEAPLAYERS_SHARED_H_
#define GALLOCY_HEAPLAYERS_SHARED_H_

#include "heaplayers/singleton.h"


namespace HL {

class SharedPageTableHeap {
 public:
  static SharedPageTableHeap& getInstance() {
    static SharedPageTableHeap instance;
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
  static SingletonHeapType heap;
  // Need public for STL allocators.
  // SharedPageTableHeap() {};
  // SharedPageTableHeap(SharedPageTableHeap const&);
  // void operator=(SharedPageTableHeap const&);
};

}  // namespace HL

#endif  // GALLOCY_HEAPLAYERS_SHARED_H_
