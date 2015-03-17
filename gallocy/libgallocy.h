#ifndef _LIBGALLOCY_H
#define _LIBGALLOCY_H

#include "pagetable.h"
#include "firstfitheap.h"

#include "heaplayers/heaptypes.h"


extern MainHeapType heap;


extern SingletonHeapType singletonHeap;


extern "C" {

  void *internal_malloc(size_t);
  void internal_free(void *);
  void *internal_realloc(void *, size_t);
  char *internal_strdup(const char *);
  void *internal_calloc(size_t, size_t);

}


extern "C" {

  void __reset_memory_allocator();
  void* custom_malloc(size_t);
  void custom_free(void*);
  void* custom_realloc(void*, size_t);
  // This is an OSX thing, but is useful for testing.
  size_t custom_malloc_usable_size(void*);
#ifdef __APPLE__
  void custom_malloc_lock();
  void custom_malloc_unlock();
#endif

}

#endif
