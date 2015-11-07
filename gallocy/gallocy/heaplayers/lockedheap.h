#ifndef GALLOCY_HEAPLAYERS_LOCKEDHEAP_H_
#define GALLOCY_HEAPLAYERS_LOCKEDHEAP_H_

#include "heaplayers/guard.h"

namespace HL {

template <class LockType, class Super>
class LockedHeap : public Super {
 public:
  inline void *malloc(size_t sz) {
    Guard<LockType> l(thelock);
    return Super::malloc(sz);
  }

  inline void free(void *ptr) {
    Guard<LockType> l(thelock);
    Super::free(ptr);
  }

  inline size_t getSize(void *ptr) const {
    Guard<LockType> l(thelock);
    return Super::getSize(ptr);
  }

  inline void lock() {
    thelock.lock();
  }

  inline void unlock() {
    thelock.unlock();
  }

 private:
  LockType thelock;
};

}  // namespace HL

#endif  // GALLOCY_HEAPLAYERS_LOCKEDHEAP_H_
