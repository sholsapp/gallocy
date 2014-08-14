#ifndef _PAGETABLEHEAP_H_
#define _PAGETABLEHEAP_H_

#include "pagetable.h"


namespace HL {

template <class SuperHeap>
class PageTableHeap : public SuperHeap {
public:

  inline void * malloc (size_t sz) {
    void *ptr;
    ptr = SuperHeap::malloc (sz);
    pt.insert_page_table_entry(ptr);
    return ptr;
  }

  inline void free (void *ptr) {
    SuperHeap::free(ptr);
  }

};

}

#endif
