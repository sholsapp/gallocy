#ifndef _DEBUGHEAP_H_
#define _DEBUGHEAP_H_

#include <assert.h>


namespace HL {

template <class Super,
          char allocChar = 'A',
	  char freeChar = 'F'>
class DebugHeap : public Super {

  private:

    enum { CANARY = 0xdeadbeef };

  public:

    inline void * malloc (size_t sz) {
      void * ptr;
      ptr = Super::malloc (sz);
      //ptr = Super::malloc (sz + sizeof(unsigned long));
      for (unsigned int i = 0; i < sz; i++) {
        ((char *) ptr)[i] = allocChar;
      }

      //*((unsigned long *) ((char *) ptr + sz)) = (unsigned long) CANARY;
      //assert (Super::getSize(ptr) >= sz);

      return ptr;
    }

    inline void free (void * ptr) {

      if (!ptr)
        return;

      //char * b = (char *) ptr;
      size_t sz = Super::getSize(ptr);

      // Check for the canary.
      //unsigned long storedCanary = *((unsigned long *) b + sz - sizeof(unsigned long));
      //if (storedCanary != CANARY) {
      //  abort();
      //}

      for (int i = 0; i < sz; i++) {
        ((char *) ptr)[i] = freeChar;
      }
      Super::free (ptr);
    }
  };

}

#endif
