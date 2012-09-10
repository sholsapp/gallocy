#include <cstdio>
#include <cstdlib>
#include <sys/mman.h>

#include "heaps/source.h"

class MainHeap : public SourceHeap<SimpleHeap> {};

MainHeap heap;

extern "C" {

  void* custom_malloc(size_t sz) {
    return heap.malloc(sz);
  }

  void custom_free(void* ptr) {
    heap.free(ptr);
  }

}
