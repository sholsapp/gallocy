#ifndef _LIBGALLOCY_H
#define _LIBGALLOCY_H

#include "pagetable.h"
#include "heaplayers/pagetableheap.h"
#include "heaplayers/source.h"


class MainHeap: public HL::PageTableHeap<SourceHeap> {};


extern MainHeap heap;


extern "C" {

  void* custom_malloc(size_t);
  void custom_free(void*);
  void* custom_realloc(void*, size_t);

#ifdef __APPLE__

  size_t custom_malloc_usable_size(void*);
  void custom_malloc_lock();
  void custom_malloc_unlock();

#endif

}

#endif
