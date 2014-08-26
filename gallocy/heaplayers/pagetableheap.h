#ifndef _PAGETABLEHEAP_H_
#define _PAGETABLEHEAP_H_

#include "pagetable.h"


namespace HL {

template <class SuperHeap>
class PageTableHeap : public SuperHeap {
public:

  PageTableHeap() {
    sum = 0;
  }

  inline void * malloc (size_t sz) {
    void *ptr;
    ptr = SuperHeap::malloc (sz);
    if (ptr)
      pt.insert_page_table_entry(ptr, sz);
      sum += sz;
    return ptr;
  }

  inline void free (void *ptr) {
    SuperHeap::free(ptr);
  }

private:
  long int sum;

};

}

#endif
