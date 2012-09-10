#include <cstring>
#include <cstdlib>
#include <sys/mman.h>

#include <map>
#include "heaps/stl.h"
#include "heaps/util/linkedlist.h"

#define ZONE_SZ   4096 * 4096 * 16
#define MMAP_PROT PROT_READ|PROT_WRITE
#define MMAP_FLAG MAP_ANON|MAP_SHARED

class SimpleHeap {
  /**
   * TODO: Tell everyone why they shouldn't use SimpleHeap: it is a proof of concept
   *       and nothing else. =)
   */
  public:
    inline void* malloc(size_t sz) {
      /**
       * Allocate sz many byte(s) of memory.
       */
      void* mem = NULL;
      if (!zone) {
        if ((zone = mmap(NULL, ZONE_SZ, MMAP_PROT, MMAP_FLAG, -1, 0)) == MAP_FAILED) {
          fprintf(stderr, "MAP FAILED\n");
          return NULL;
        }
        next = (char*) zone;
        fprintf(stderr, "> OS gives %d bytes(s) at %p\n", ZONE_SZ, zone);
      }
      mem  = (void*) next;
      next = next + sz;
      fprintf(stderr, "> Allocating %d byte(s) at %p\n", sz, mem);
      return mem;
    }

    inline void free(void* ptr) {
      /**
       * Unimplemented method stub.
       */
      fprintf(stderr, "> Freeing %p\n", ptr);
    }

  private:
    void* zone;
    char* next;
};

typedef
  std::map<void*, int,
  std::less<void*>,
  STLAllocator<std::pair<void*, int>, SimpleHeap> >
    MyMap2;

template <class SuperHeap>
class SourceHeap : public SuperHeap {
  /**
   * Keep track of the size of each ptr.
   */
  public:
    SourceHeap() {
      fprintf(stderr, "> create\n");
    }

    ~SourceHeap() {
      fprintf(stderr, "> destroy\n");
    }

    inline void* malloc(size_t sz) {
      void* ptr = SuperHeap::malloc(sz);
      acct[ptr] = sz;
      return ptr;
    }

    inline void free(void* ptr) {
      MyMap2::iterator it = acct.find(ptr);
      if (it != acct.end())
        fprintf(stderr, "Pointer %p is in the map.\n", ptr);
      list.insert((int*) &ptr);
    }

  private:
    MyMap2 acct;
    LinkedList list;

};
