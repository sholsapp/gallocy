#include <cstring>
#include <cstdlib>
#include <sys/mman.h>

#include <map>
#include <vector>


#define ZONE_SZ   4096 * 16 * 2
#define MMAP_PROT PROT_READ|PROT_WRITE
#define MMAP_FLAG MAP_ANON|MAP_SHARED


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
        fprintf(stderr, "> Allocator(%p) has bytes_left=%lu\n", this, bytes_left);
      }
      if (bytes_left > sz) {
        mem  = (void*) next;
        next = next + sz;
        bytes_left -= sz;
        fprintf(stderr, "> Allocated %zu byte(s) at %p\n", sz, mem);
        fprintf(stderr, "> Allocator(%p) has bytes_left=%lu\n", this, bytes_left);
        return mem;
      }
      else {
        fprintf(stderr, "> Exhausted virtual memory\n");
        fprintf(stderr, "> Allocator(%p) has bytes_left=%lu\n", this, bytes_left);
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
    unsigned long bytes_left;
};


class SingletonHeap {
  public:
    static SingletonHeap& getInstance() {
      static SingletonHeap instance;
      return instance;
    }

    static void* malloc(size_t sz) {
      return heap.malloc(sz);
    }

    static void free(void* ptr) {
      heap.free(ptr);
    }

  private:
    static SimpleHeap heap;

    // Need public for STL allocators...
    //
    //SingletonHeap() {};
    //SingletonHeap(SingletonHeap const&);
    //void operator=(SingletonHeap const&);

};

SimpleHeap singletonHeap;
SimpleHeap SingletonHeap::heap = singletonHeap;
