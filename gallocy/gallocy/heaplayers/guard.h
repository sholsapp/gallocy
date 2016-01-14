#ifndef GALLOCY_HEAPLAYERS_GUARD_H_
#define GALLOCY_HEAPLAYERS_GUARD_H_
#include <iostream>
namespace HL {

template <class LockType>
class Guard {
 public:
  inline explicit Guard(LockType &l)
      : _lock(l) {
    _lock.lock();
  }

  inline ~Guard(void) {
    _lock.unlock();
  }
 private:
  LockType &_lock;
};

}  // namespace HL

#endif  // GALLOCY_HEAPLAYERS_GUARD_H_
