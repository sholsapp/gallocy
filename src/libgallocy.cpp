#include <cstdio>
#include <cstdlib>
#include <sys/mman.h>

#include "libgallocy.h"


volatile int anyThreadCreated = 0;


extern "C" {

  void* custom_malloc(size_t sz) {
    void* ptr = heap.malloc(sz);
    return ptr;
  }

  void custom_free(void* ptr) {
    heap.free(ptr);
  }

  void* custom_realloc(void* ptr, size_t sz) {
    if (ptr == NULL) {
      return custom_malloc(sz);
    }
    size_t min_size = heap.getSize(ptr);
    void* buf = custom_malloc(sz);
    if (buf != NULL) {
      memcpy(buf, ptr, min_size);
      custom_free(ptr);
    }
    return buf;
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
