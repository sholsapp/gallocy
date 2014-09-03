#ifndef _ADDHEAP_H_
#define _ADDHEAP_H_

// Reserve space for a class in the head of every allocated object.

namespace HL {

template <class Add, class Super>
class AddHeap : public Super {
public:

  inline void * malloc (size_t sz) {
    void * ptr = Super::malloc (sz + align(sizeof(Add)));
    void * newPtr = (void *) align ((size_t) ((Add *) ptr + 1));
    return newPtr;
  }

  inline void free (void * ptr) {
    void * origPtr = (void *) ((Add *) ptr - 1);
    Super::free (origPtr);
  }

  inline size_t getSize (void * ptr) {
    void * origPtr = (void *) ((Add *) ptr - 1);
    return Super::getSize (origPtr);
  }

private:
  static inline size_t align (size_t sz) {
    return (sz + (sizeof(double) - 1)) & ~(sizeof(double) - 1);
  }

};

};
#endif
