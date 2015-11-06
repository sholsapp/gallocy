#ifndef GALLOCY_HEAPLAYERS_ZONEHEAP_H_
#define GALLOCY_HEAPLAYERS_ZONEHEAP_H_

#include <assert.h>


namespace HL {

template <class Super, size_t ChunkSize>
class ZoneHeap : public Super {
 public:
  ZoneHeap(void)
    : sizeRemaining(-1),
      currentArena(NULL),
      pastArenas(NULL) {}

  ~ZoneHeap(void) {
    __reset();
  }

  inline void *malloc(size_t sz) {
    void *ptr = zoneMalloc(sz);
    return ptr;
  }

  inline void free(void *ptr) {
    return;
  }

  inline int remove(void *ptr) {
    return 0;
  }

  inline void __reset() {
    // Delete all of our arenas.
    Arena *ptr = pastArenas;
    while (ptr != NULL) {
      void * oldPtr = reinterpret_cast<void *>(ptr);
      ptr = ptr->nextArena;
      Super::free(oldPtr);
    }
    if (currentArena != NULL)
      Super::free(currentArena);
    Super::__reset();
  }

 private:
  inline static size_t align(int sz) {
    return (sz + (sizeof(double) - 1)) & ~(sizeof(double) - 1);
  }

  inline void *zoneMalloc(size_t sz) {
    void *ptr;
    // Round up size to an aligned value.
    sz = align(sz);
    // Get more space in our arena if there's not enough room in this one.
    if ((currentArena == NULL) || (sizeRemaining < reinterpret_cast<uint64_t>(sz))) {
      // First, add this arena to our past arena list.
      if (currentArena != NULL) {
        currentArena->nextArena = pastArenas;
        pastArenas = currentArena;
      }
      // Now get more memory.
      size_t allocSize = ChunkSize;
      if (allocSize < sz) {
        allocSize = sz;
      }
      currentArena =
        reinterpret_cast<Arena *>(Super::malloc(allocSize + sizeof(Arena)));
      if (currentArena == NULL) {
        return NULL;
      }
      currentArena->arenaSpace = reinterpret_cast<char *>(currentArena + 1);
      currentArena->nextArena = NULL;
      sizeRemaining = ChunkSize;
    }
    // Bump the pointer and update the amount of memory remaining.
    sizeRemaining -= sz;
    ptr = currentArena->arenaSpace;
    currentArena->arenaSpace += sz;
    assert(ptr != NULL);
    return ptr;
  }

  class Arena {
   public:
      Arena * nextArena;
      char * arenaSpace;
      double _dummy;  // For alignment.
  };

  // Space left in the current arena.
  uint64_t sizeRemaining;
  // The current arena.
  Arena * currentArena;
  // A linked list of past arenas.
  Arena * pastArenas;
};

}  // namespace HL

#endif  //  GALLOCY_HEAPLAYERS_ZONEHEAP_H_
