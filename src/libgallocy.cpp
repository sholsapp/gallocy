#include <cstdio>
#include <cstdlib>
#include <sys/mman.h>

volatile int anyThreadCreated = 0;

#include "heaplayers/myhashmap.h"
#include "heaplayers/spinlock.h"
#include "heaplayers/lockedheap.h"
#include "heaplayers/freelistheap.h"
#include "heaplayers/firstfitheap.h"
#include "heaplayers/zoneheap.h"
#include "heaplayers/source.h"


class MainHeap: public SourceHeap {};

MainHeap heap;

extern "C" {

  void* custom_malloc(size_t sz) {
    fprintf(stderr, "custom_malloc\n");
    return heap.malloc(sz);
  }

  void custom_free(void* ptr) {
    heap.free(ptr);
  }

#ifdef __APPLE__

  size_t custom_malloc_usable_size(void* ptr) {
    // Hopefully we don't need these yet!
    return -1;
  }

  void custom_malloc_lock() {
    // Hopefully we don't need these yet!
    return;
  }

  void custom_malloc_unlock() {
    // Hopefully we don't need these yet!
    return;
  }

#endif

}
