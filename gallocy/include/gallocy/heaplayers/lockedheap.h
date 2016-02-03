#ifndef GALLOCY_HEAPLAYERS_LOCKEDHEAP_H_
#define GALLOCY_HEAPLAYERS_LOCKEDHEAP_H_

#include <pthread.h>

#include "heaplayers/guard.h"

namespace HL {

template <class LockType, class Super>
class LockedHeap : public Super {
 public:

   LockedHeap() {
     alock = PTHREAD_MUTEX_INITIALIZER;
   }

  inline void *malloc(size_t sz) {
    //Guard<LockType> l(thelock);
    Guard<pthread_mutex_t> l(alock);
    return Super::malloc(sz);
  }

  inline void free(void *ptr) {
    //Guard<LockType> l(thelock);
    Guard<pthread_mutex_t> l(alock);
    Super::free(ptr);
  }

  inline void *realloc(void *ptr, size_t sz) {
    Guard<pthread_mutex_t> l(alock);
    return Super::realloc(ptr, sz);
  }

  inline char *strdup(const char *s1) {
    Guard<pthread_mutex_t> l(alock);
    return Super::strdup(s1);
  }

  inline void *calloc(size_t count, size_t size) {
    Guard<pthread_mutex_t> l(alock);
    return Super::calloc(count, size);
  }

  inline size_t getSize(void *ptr) {
    //Guard<LockType> l(thelock);
    Guard<pthread_mutex_t> l(alock);
    return Super::getSize(ptr);
  }

  inline void lock() {
    //thelock.lock();
    pthread_mutex_lock(&alock);
  }

  inline void unlock() {
    //thelock.unlock();
    pthread_mutex_unlock(&alock);
  }

 private:
  LockType thelock;
  pthread_mutex_t alock;
};

}  // namespace HL

#endif  // GALLOCY_HEAPLAYERS_LOCKEDHEAP_H_
