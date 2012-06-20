#include <cstdio>
#include <cstdlib>


class MmapHeap {
  public:
    inline void* malloc(size_t sz) {
      fprintf(stderr, "> MmapHeap > malloc: Allocating %d byte(s)\n", sz);
      return NULL;
    }
    inline void free(void* ptr) {
      fprintf(stderr, "> MmapHeap > free: Freeing %p\n", ptr);
      return;
    }
  private:
    void* zone;
};

class MainHeap : public MmapHeap {};

MainHeap heap;

extern "C" {
  void* custom_malloc(size_t sz) {
    fprintf(stderr, "> custom_malloc: Allocating %d byte(s)\n", sz);
    return heap.malloc(sz);
  }
  void custom_free(void* ptr) {
    fprintf(stderr, "> custom_free: Freeing %p\n", ptr);
    heap.free(ptr);
    return;
  }
}
