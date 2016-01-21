#ifndef GALLOCY_HEAPLAYERS_FIRSTFITHEAP_H_
#define GALLOCY_HEAPLAYERS_FIRSTFITHEAP_H_

#include <cstdio>

namespace HL {

template <class Super>
class FirstFitHeap : public Super {
 public:
  FirstFitHeap(void)
    : myFreeList(NULL), nObjects(0) {
  }

  ~FirstFitHeap(void) {
    __reset();
  }

  inline void *malloc(size_t sz) {
    // Check the free list first.
    void * ptr = myFreeList;
    if (ptr == NULL) {
      ptr = Super::malloc(sz);
    } else {
      freeObject *p = myFreeList;
      freeObject *prev = NULL;
      while ((p != NULL) && (Super::getSize(reinterpret_cast<void *>(p)) < sz)) {
        prev = p;
        p = p->next;
      }
      if (p == NULL) {
        ptr = Super::malloc(sz);
      } else {
        if (prev == NULL) {
          myFreeList = p->next;
        } else {
          prev->next = p->next;
        }
        nObjects--;
        ptr = p;
      }
    }
    return ptr;
  }

  inline void free(void *ptr) {
    if (!ptr)
      return;

    for (unsigned int i = 0; i < Super::getSize(reinterpret_cast<void *>(ptr)); i++) {
      reinterpret_cast<unsigned char *>(ptr)[i] = 0xED;
    }

    // Add this object to the free list.
    nObjects++;
    freeObject *p = myFreeList;
    freeObject *prev = NULL;
    // Insert the object "in order".
#if 1
    while ((p != NULL) & (p <= ptr)) {
      prev = p;
      p = p->next;
    }
#endif
    if (prev == NULL) {
      (reinterpret_cast<freeObject *>(ptr))->next = myFreeList;
      myFreeList = reinterpret_cast<freeObject *>(ptr);
    } else {
      (reinterpret_cast<freeObject *>(ptr))->next = prev->next;
      prev->next = reinterpret_cast<freeObject *>(ptr);
    }
  }

  inline void __reset() {
    void *ptr = myFreeList;
    while (ptr != NULL) {
      void *oldptr = ptr;
      ptr = reinterpret_cast<void *>((reinterpret_cast<freeObject *>(ptr))->next);
      Super::free(oldptr);
      --nObjects;
    }
    myFreeList = NULL;
    nObjects = 0;
    Super::__reset();
  }

 private:
  int classInvariant() {
    return (((myFreeList == NULL) && (nObjects == 0))
        || ((myFreeList != NULL) && (nObjects > 0)));
  }

  class freeObject {
   public:
    freeObject * next;
  };

  freeObject *myFreeList;
  int nObjects;
};

}  // namespace HL

#endif  // GALLOCY_HEAPLAYERS_FIRSTFITHEAP_H_
