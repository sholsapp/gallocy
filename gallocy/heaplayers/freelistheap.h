#ifndef GALLOCY_HEAPLAYERS_FREELISTHEAP_H_
#define GALLOCY_HEAPLAYERS_FREELISTHEAP_H_

#include <assert.h>

#include "heaplayers/freesllist.h"

#ifndef NULL
#define NULL 0
#endif

namespace HL {

template <class SuperHeap>
class FreelistHeap : public SuperHeap {
 public:
  inline void *malloc(size_t sz) {
    // Check the free list first.
    void *ptr = _freelist.get();
    // If it's empty, get more memory; otherwise, advance the free list
    // pointer.
    if (ptr == 0) {
      ptr = SuperHeap::malloc(sz);
    }
    return ptr;
  }

  inline void free(void *ptr) {
    if (ptr == 0) {
      return;
    }
    _freelist.insert(ptr);
  }

  inline void clear() {
    void *ptr;
    while ((ptr = _freelist.get())) {
      SuperHeap::free(ptr);
    }
  }

 private:
  FreeSLList _freelist;
};

}  // namespace HL

#endif  // GALLOCY_HEAPLAYERS_FREELISTHEAP_H_
