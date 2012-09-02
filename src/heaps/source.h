#include <cstring>
#include <cstdlib>
#include <sys/mman.h>

#define ZONE_SZ   4096
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
      fprintf(stderr, "> Allocating %d byte(s)\n", sz);
      if (!zone) {
        if ((zone = mmap(NULL, ZONE_SZ, MMAP_PROT, MMAP_FLAG, -1, 0)) == MAP_FAILED) {
          fprintf(stderr, "MAP FAILED\n");
          return NULL;
        }
        next = (char*) zone;
      }
      mem  = (void*) next;
      next = next + sz;
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

