#ifndef GALLOCY_HEAPLAYERS_SIZEHEAP_H_
#define GALLOCY_HEAPLAYERS_SIZEHEAP_H_

#include "heaplayers/addheap.h"


namespace HL {

template <class Super>
class UseSizeHeap : public Super {
 public:
  inline UseSizeHeap(void) {}

  inline static size_t getSize(void *ptr) {
    return (reinterpret_cast<freeObject *>(ptr) - 1)->sz;
  }

 protected:
  struct freeObject {
    uint64_t _dummy;  // for alignment.
    uint64_t sz;
  };
};


template <class SuperHeap>
class SizeHeap : public UseSizeHeap<SuperHeap> {
  typedef typename UseSizeHeap<SuperHeap>::freeObject freeObject;
 public:
  inline SizeHeap(void) {}

  inline void *malloc(const size_t sz) {
    // Add room for a size field.
    freeObject *ptr = reinterpret_cast<freeObject *>(SuperHeap::malloc(sz + sizeof(freeObject)));
    // Store the requested size.
    ptr->sz = sz;
    return reinterpret_cast<void *>(ptr + 1);
  }

  inline void free(void *ptr) {
    SuperHeap::free(reinterpret_cast<freeObject *>(ptr) - 1);
  }

  inline void __reset() {
    SuperHeap::__reset();
  }
};

}  // namespace HL

#endif  // GALLOCY_HEAPLAYERS_SIZEHEAP_H_
