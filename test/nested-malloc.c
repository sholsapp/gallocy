/**
 * The hotspot JRE uses `malloc`, scoped in a class `os`.  This tests checks
 * to see what happens when an application uses a custom malloc.
 *
 * See: <hotspot>/src/share/vm/runtime/os.hpp
 */
#include <cstdlib>
#include <cstdio>

namespace app {
  void* malloc(size_t sz) {
    fprintf(stderr, "app malloc %d\n", sz);
    return ::malloc(sz);
  }
}

int main(int argc, char *argv[]) {
  fprintf(stderr, "Starting\n");
  char* m = (char*) app::malloc(16);
  m[0] = 'a';
  fprintf(stderr, "Ending\n");
  return 0;
}
