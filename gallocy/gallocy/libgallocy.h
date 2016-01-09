#ifndef GALLOCY_LIBGALLOCY_H_
#define GALLOCY_LIBGALLOCY_H_

#include <functional>
#include <map>
#include <sstream>
#include <streambuf>
#include <string>
#include <utility>
#include <vector>

#include "external/json.hpp"
#include "heaplayers/firstfitheap.h"
#include "heaplayers/heaptypes.h"
#include "allocators/internal.h"
#include "allocators/shared.h"


extern volatile int anyThreadCreated;


extern MainHeapType heap;


extern "C" {
  void __reset_memory_allocator();
  void* custom_malloc(size_t sz);
  void custom_free(void *ptr);
  void* custom_realloc(void *ptr, size_t sz);
  // This is an OSX thing, but is useful for testing.
  size_t custom_malloc_usable_size(void *ptr);
#ifdef __APPLE__
  void custom_malloc_lock();
  void custom_malloc_unlock();
#endif

}

#endif  // GALLOCY_LIBGALLOCY_H_
