#include "gallocy/utils/constants.h"

#include <iostream>


extern "C" {
extern char* main;
#ifdef __linux__
extern char* _end;
#elif __APPLE__
// TODO(sholsapp): find a way to calculate this dynamically. This is a hack for
// OS X only. This affects where the internal allocator is instantiated.
char* _end = reinterpret_cast<char *>(0x800000000);
#endif
}


uint64_t *&global_main() {
  static uint64_t *m = reinterpret_cast<uint64_t *>(&main);
  return m;
}


uint64_t *&global_end() {
  static uint64_t *e = reinterpret_cast<uint64_t *>(&_end);
  return e;
}


uint64_t *&global_base() {
  // Page align the _end of the program + a page
  static uint64_t *base = reinterpret_cast<uint64_t *>((((uint64_t) &_end) & ~(PAGE_SZ - 1)) + PAGE_SZ);
  return base;
}

void *get_heap_location(uint64_t purpose) {
  switch (purpose) {
    case PURPOSE_DEVELOPMENT_HEAP:
      std::cout << "[development heap]" << std::endl;
      return nullptr;
      break;
    case PURPOSE_INTERNAL_HEAP:
      return reinterpret_cast<void *>(global_base() + ZONE_SZ * 0);
      break;
    case PURPOSE_SHARED_HEAP:
      return reinterpret_cast<void *>(global_base() + ZONE_SZ * 1);
      break;
    case PURPOSE_APPLICATION_HEAP:
      return reinterpret_cast<void *>(global_base() + ZONE_SZ * 2);
      break;
    default:
      std::cout << "[unknown heap]" << std::endl;
      return nullptr;
      break;
  }
}
