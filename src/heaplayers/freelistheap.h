#ifndef _FREELISTHEAP_H_
#define _FREELISTHEAP_H_

#include "freesllist.h"
#include <assert.h>

#ifndef NULL
#define NULL 0
#endif

namespace HL {

template <class SuperHeap>
class FreelistHeap : public SuperHeap {
public:

  inline void * malloc (size_t sz) {
    // Check the free list first.
    void * ptr = _freelist.get();
    // If it's empty, get more memory;
    // otherwise, advance the free list pointer.
    if (ptr == 0) {
      //fprintf(stderr, "free list (%d) getting memory from parent (%d)\n", sz, (int) this);
      ptr = SuperHeap::malloc (sz);
    }
    return ptr;
  }

  inline void free (void * ptr) {
    if (ptr == 0) {
      return;
    }
    _freelist.insert (ptr);
  }

  inline void clear (void) {
    void * ptr;
    while (ptr = _freelist.get()) {
      SuperHeap::free (ptr);
    }
  }

private:

  FreeSLList _freelist;

};

}

#endif
