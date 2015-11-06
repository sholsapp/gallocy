#ifndef GALLOCY_HEAPLAYERS_DEBUGHEAP_H_
#define GALLOCY_HEAPLAYERS_DEBUGHEAP_H_

#include <assert.h>


// XXX: Some code in this file is disabled because it opaquely changes the size
// of the allocated point size. This is okay, functionally, cut makes testing very
// awkward by requiring that when we allocate x bytes that we check for (x + 8)
// bytes. For now, disable functionality that increases the allocaiton.


namespace HL {

template <class Super, char allocChar = 'A', char freeChar = 'F'>
class DebugHeap : public Super {
 private:
  enum { CANARY = 0xdeadbeef };

 public:
  inline void *malloc(size_t sz) {
    void *ptr;
    ptr = Super::malloc(sz);
#if 0
    ptr = Super::malloc(sz + sizeof(uint64_t));
    for (unsigned int i = 0; i < sz; i++) {
      reinterpret_cast<char *>(ptr)[i] = allocChar;
    }
    *(reinterpret_cast<uint64_t *>(reinterpret_cast<char *>(ptr + sz))) = reinterpret_cast<uint64_t>(CANARY);
    assert(Super::getSize(ptr) >= sz);
#endif
    return ptr;
  }

  inline void free(void *ptr) {
    if (!ptr) {
      return;
    }
    char *b = reinterpret_cast<char *>(ptr);
    size_t sz = Super::getSize(ptr);
#if 0
    // Check for the canary.
    uint64_t storedCanary = *(reinterpret_cast<uint64_t *>(b + sz - sizeof(uint64_t)));
    if (storedCanary != CANARY) {
      abort();
    }
#endif
    for (uint64_t i = 0; i < sz; i++) {
      reinterpret_cast<char *>(ptr)[i] = freeChar;
    }
    Super::free(ptr);
  }
};

}  // namespace HL

#endif  // GALLOCY_HEAPLAYERS_DEBUGHEAP_H_
