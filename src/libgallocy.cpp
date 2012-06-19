#include <cstdio>
#include <cstdlib>


class MmapHeap {
  inline void* malloc(size_t sz) {
    return NULL;
  }
  inline void free(void* ptr) {
    return;
  }
};

class MainHeap : public MmapHeap {};

extern "C" {
  void* custom_malloc(size_t) {
    return NULL;
  }
  void custom_free(void* ptr) {
    return;
  }
}
