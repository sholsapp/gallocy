#include "gallocy/constants.h"


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


/**
 * Return the address of the ``main`` function.
 */
uint64_t *&global_main() {
  static uint64_t *m = reinterpret_cast<uint64_t *>(&main);
  return m;
}


/**
 * Return the address that demarks the end of the .TEXT region.
 */
uint64_t *&global_end() {
  static uint64_t *e = reinterpret_cast<uint64_t *>(&_end);
  return e;
}


/**
 * Return the starting address for use by internal memory allocators.
 */
uint64_t *&global_base() {
  // Page align the _end of the program + a page
  static uint64_t *base = reinterpret_cast<uint64_t *>((((uint64_t) &_end) & ~(PAGE_SZ - 1)) + PAGE_SZ);
  return base;
}
