#ifndef _SOURCE_H
#define _SOURCE_H


#include <sys/mman.h>

#include "myhashmap.h"
#include "spinlock.h"
#include "lockedheap.h"
#include "freelistheap.h"
#include "firstfitheap.h"
#include "zoneheap.h"
#include "sizeheap.h"
#include "heaptypes.h"


#define ZONE_SZ   4096 * 4096
#define MMAP_PROT PROT_READ|PROT_WRITE
#define MMAP_FLAG MAP_ANON|MAP_SHARED


namespace HL {


class SourceMmapHeap {

  public:

    inline void* malloc(size_t sz) {

      void* mem = NULL;

      if (!zone) {

        if ((zone = mmap(NULL, ZONE_SZ, MMAP_PROT, MMAP_FLAG, -1, 0)) == MAP_FAILED)
          abort();

        next = (char*) zone;
        bytes_left = ZONE_SZ;

      }

      if (bytes_left > sz) {
        mem  = (void*) next;
        next = next + sz;
        bytes_left -= sz;
        return mem;
      }
      else {
        abort();
      }

    }

    inline void free(void* ptr) {
      //munmap(reinterpret_cast<char*>(ptr), getSize(ptr));
      return;
    }

    inline void free(void* ptr, size_t sz) {
      //munmap(reinterpret_cast<char*>(ptr), getSize(ptr));
      return;
    }

    inline size_t getSize(void* ptr) {
      return -1;
    }

    inline void __reset() {
      zone = NULL;
      next = NULL;
      bytes_left = 0;
    }

  private:
    void* zone;
    char* next;
    unsigned long bytes_left;
};

#if 0
typedef
  HL::LockedHeap<
    HL::SpinLockType,
    HL::FirstFitHeap<
      HL::SizeHeap<
        HL::ZoneHeap<
          SourceMmapHeap,
          16384 - 16> > > >
  InternalSourceHeapType;


class SourceHeap: public SourceMmapHeap {
  public:
    inline void* malloc(size_t sz) {
      void* ptr = SourceMmapHeap::malloc(sz);
      MyMapLock.lock();
      MyMap.set (ptr, sz);
      MyMapLock.unlock();
      //assert (reinterpret_cast<size_t>(ptr) % 4096 == 0);
      return const_cast<void*>(ptr);
    }

    inline size_t getSize(void* ptr) {
      MyMapLock.lock();
      size_t sz = MyMap.get(ptr);
      MyMapLock.unlock();
      return sz;
    }

    // WORKAROUND: apparent gcc bug.
    void free(void* ptr, size_t sz) {
      SourceMmapHeap::free (ptr, sz);
    }

    inline void free(void* ptr) {
      //assert (reinterpret_cast<size_t>(ptr) % 4096 == 0);
      MyMapLock.lock();
      size_t sz = MyMap.get(ptr);
      SourceMmapHeap::free(ptr, sz);
      MyMap.erase(ptr);
      MyMapLock.unlock();
    }

  private:

    class MyHeap : public InternalSourceHeapType {};
    typedef HL::MyHashMap<void*, size_t, MyHeap> mapType;

  protected:

    mapType MyMap;

    HL::SpinLockType MyMapLock;

};
#endif


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

    static void __reset() {
      heap.__reset();
    }

  private:
    static SingletonHeapType heap;

    // Need public for STL allocators.
    //
    //SingletonHeap() {};
    //SingletonHeap(SingletonHeap const&);
    //void operator=(SingletonHeap const&);
};

}

#endif
