#include "gallocy/libgallocy.h"

#include <sys/mman.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>


// When we start handling multithreaded applications, this must be set to `1`
// when a thread is created. This is for the HL::SpinLockType.
volatile int anyThreadCreated = 1;


// This is the main heap that we expose to the application.
ApplicationHeapType heap;


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
    return heap.malloc(sz);
  }

  void custom_free(void* ptr) {
    heap.free(ptr);
  }

  void* custom_realloc(void* ptr, size_t sz) {
    return heap.realloc(ptr, sz);
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
