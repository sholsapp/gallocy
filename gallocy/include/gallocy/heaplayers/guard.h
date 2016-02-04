#ifndef GALLOCY_HEAPLAYERS_GUARD_H_
#define GALLOCY_HEAPLAYERS_GUARD_H_

#include <pthread.h>

#include <iostream>

namespace HL {

template <class LockType>
class Guard {
 public:
  inline explicit Guard(LockType &l)
      : _lock(l) {
    pthread_mutex_lock(&_lock);
    // _lock.lock();
  }

  inline ~Guard(void) {
    // _lock.unlock();
    pthread_mutex_unlock(&_lock);
  }
 private:
  LockType &_lock;
};

}  // namespace HL

#endif  // GALLOCY_HEAPLAYERS_GUARD_H_
