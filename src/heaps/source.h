#include <cstring>
#include <cstdlib>
#include <sys/mman.h>

#include <map>
#include <vector>
#include "heaps/stl.h"
#include "heaps/util/linkedlist.h"

#define ZONE_SZ   4096 * 4096 * 2
#define MMAP_PROT PROT_READ|PROT_WRITE
#define MMAP_FLAG MAP_ANON|MAP_SHARED

/**
 * A `SimpleHeap` talks to the operating system to get memory.
 *
 * A `SimpleHeap` never free's memory once it is mapped.
 *
 */
class SimpleHeap {
  public:
    inline void* malloc(size_t sz) {
      void* mem = NULL;
      if (!zone) {
        if ((zone = mmap(NULL, ZONE_SZ, MMAP_PROT, MMAP_FLAG, -1, 0)) == MAP_FAILED) {
          fprintf(stderr, "MAP FAILED\n");
          return NULL;
        }
        next = (char*) zone;
        bytes_left = ZONE_SZ;
        fprintf(stderr, "> OS gives %d bytes(s) at %p\n", ZONE_SZ, zone);
      }
      if (bytes_left > 0) {
        mem  = (void*) next;
        next = next + sz;
        bytes_left -= sz;
        fprintf(stderr, "> Allocating %d byte(s) at %p\n", sz, mem);
        return mem;
      }
      else {
        fprintf(stderr, "> Exausted virtual memory\n");
        return NULL;
      }
    }

    inline void free(void* ptr) {
      // A `SimpleHeap` never free's memory once it is mapped.
      return;
    }

  private:
    void* zone;
    char* next;
    int bytes_left;
};


/**
 * A `FreelistHeap` will attempt to use a list of previously free'd
 * pointers to satisfy calls to malloc before asking it's `SuperHeap`
 * for more memory.
 *
 * A `FreelistHeap` assumes that the pointers maintained in the list
 * point to chunks of memory that are the same size.
 *
 */
template <class SuperHeap>
class FreelistHeap : public SuperHeap {
  public:
    inline void* malloc(size_t sz) {
      void* ptr = list.remove();
      if (ptr == NULL) {
        ptr = SuperHeap::malloc(sz);
      }
      return ptr;
    }

    inline void free(void* ptr) {
      list.insert(ptr);
    }

  private:
    LinkedList list;
};

/**
 * A `MyMap2`'s allocator always asks for chunks of memory of the same
 * size, so using a `FreelistHeap` is okay.
 */
typedef
  std::map<void*, size_t,
  std::less<void*>,
  STLAllocator<std::pair<void*, size_t>, FreelistHeap<SimpleHeap> > >
    MyMap2;

// TODO: not sure why using the vector in `free` causes infinite loop
//
//typedef
//  std::vector<void*,
//  STLAllocator<void*, FreelistHeap<SimpleHeap> > >
//    MyList2;

/**
 * A `SourceHeap` is an allocator that can be used by the target application.
 */
template <class SuperHeap>
class SourceHeap : public SuperHeap {
  public:
    inline void* malloc(size_t sz) {
      void* ptr = NULL;
      if (ptr == NULL) {
        ptr = SuperHeap::malloc(sz);
        acct[ptr] = sz;
      }
      //list.push_back(ptr);
      return ptr;
    }

    inline void free(void* ptr) {
      //list.push_back(ptr);
    }

  private:
    MyMap2 acct;
    //MyList2 list;
};
