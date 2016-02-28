#include <stdint.h>

#include <cstdlib>

#include "gallocy/allocators/internal.h"

/**
 * The local internal memory heap.
 *
 * The internal memory region can also be referred to as local internal memory.
 * This region of memory is a good place to put local threads, process-specific
 * data, and other things that are local to a specific process running the
 * shared application.
 *
 * This is the only region of memory that is allowed to differ between local
 * processes running the shared application, and it is:
 *
 *   - not included in the shared page table
 *   - not shared with any other process running the shared application
 */
HL::SingletonInternalHeapType local_internal_memory_heap;
HL::SingletonInternalHeapType HL::InternalMemoryHeap::heap = local_internal_memory_heap;
HL::InternalMemoryHeap local_internal_memory;

/**
 * Internal memory allocators
 *
 * These are used internally to maintain data structures, but could also be
 * referenced using the ``singletonHeap`` object directly.
 */
extern "C" {

  void *internal_malloc(size_t sz) {
    return local_internal_memory.malloc(sz);
  }

  void internal_free(void *ptr) {
    local_internal_memory.free(ptr);
  }

  void* internal_realloc(void* ptr, size_t sz) {
    return local_internal_memory.realloc(ptr, sz);
  }

  char *internal_strdup(const char *s1) {
    return local_internal_memory.strdup(s1);
  }

  void *internal_calloc(size_t count, size_t size) {
    return local_internal_memory.calloc(count, size);
  }

  size_t internal_malloc_usable_size(void *ptr) {
    return local_internal_memory.getSize(ptr);
  }

}
