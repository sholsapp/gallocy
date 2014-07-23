#include <cstdio>
#include <cstdlib>
#include <sys/mman.h>

volatile int anyThreadCreated = 0;

#include "heaplayers/myhashmap.h"
#include "heaplayers/spinlock.h"
#include "heaplayers/lockedheap.h"
#include "heaplayers/freelistheap.h"
#include "heaplayers/firstfitheap.h"
#include "heaplayers/zoneheap.h"
#include "heaplayers/source.h"


//class MainHeap :
//  public HL::LockedHeap<HL::SpinLockType, HL::FreelistHeap<HL::ZoneHeap<SimpleHeap, 16384 - 16> > > {};
//

class MainHeap: public SourceHeap {};

MainHeap heap;

extern "C" {

  void* custom_malloc(size_t sz) {
    return heap.malloc(sz);
  }

  void custom_free(void* ptr) {
    heap.free(ptr);
  }

}
