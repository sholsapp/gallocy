#include <cstdio>
#include <cstdlib>
#include <sys/mman.h>

#include "libgallocy.h"


volatile int anyThreadCreated = 0;


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
