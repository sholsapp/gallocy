#include "gallocy/libgallocy.h"

#include <sys/mman.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>


// When we start handling multithreaded applications, this must be set to `1`
// when a thread is created. This is for the HL::SpinLockType.
volatile int anyThreadCreated = 0;


// This is the main heap that we expose to the application.
MainHeapType heap;


/**
 * Application memory allocators.
 *
 * These are used in conjunction with stdlib's malloc replacement hooks.
 */
extern "C" {

  void __reset_memory_allocator() {
    heap.__reset();
    local_internal_memory.__reset();
  }

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

  size_t custom_malloc_usable_size(void* ptr) {
    return heap.getSize(ptr);
  }

#ifdef __APPLE__

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
