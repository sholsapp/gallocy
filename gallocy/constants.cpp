#include "./constants.h"


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
intptr_t &global_main() {
  static intptr_t m = ((intptr_t) &main);
  return m;
}


/**
 * Return the address that demarks the end of the .TEXT region.
 */
intptr_t &global_end() {
  static intptr_t e = ((intptr_t) &_end);
  return e;
}


/**
 * Return the starting address for use by internal memory allocators.
 */
intptr_t &global_base() {
  // Page align the _end of the program + a page
  static intptr_t base = (((intptr_t) &_end) & ~(PAGE_SZ - 1)) + PAGE_SZ;
  return base;
}
