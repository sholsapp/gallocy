#ifndef GALLOCY_HEAPLAYERS_HEAPTYPES_H_
#define GALLOCY_HEAPLAYERS_HEAPTYPES_H_


// FIX ME: 16 = size of ZoneHeap header.
#define DEFAULT_ZONE_SZ 16384 - 16


#include "heaplayers/addheap.h"
#include "heaplayers/debugheap.h"
#include "heaplayers/firstfitheap.h"
#include "heaplayers/freelistheap.h"
#include "heaplayers/lockedheap.h"
#include "heaplayers/myhashmap.h"
#include "heaplayers/pagetableheap.h"
#include "heaplayers/sizeheap.h"
#include "heaplayers/spinlock.h"
#include "heaplayers/stdlibheap.h"
#include "heaplayers/stl.h"
#include "heaplayers/zoneheap.h"


namespace HL {

class SourceMmapHeap;

}


/**
 * Local internal memory.
 */
typedef
  HL::StdlibHeap<
    HL::FirstFitHeap<
      HL::SizeHeap<
        HL::ZoneHeap<
          HL::SourceMmapHeap,
          DEFAULT_ZONE_SZ> > > >
  SingletonHeapType;


#include "heaplayers/source.h"


/**
 * Shared page table.
 */
typedef
  HL::LockedHeap<
    HL::SpinLockType,
    HL::FirstFitHeap<
      HL::SizeHeap<
        HL::ZoneHeap<
          HL::SingletonHeap,
          DEFAULT_ZONE_SZ> > > >
  SqlitexSizeHeapType;


typedef
  HL::LockedHeap<
    HL::SpinLockType,
    HL::FirstFitHeap<
      HL::SizeHeap<
        HL::ZoneHeap<
          HL::SourceMmapHeap,
          DEFAULT_ZONE_SZ> > > >
  SqliteAllocatorHeapType;


/**
 * Shared application memory.
 */
typedef
  HL::DebugHeap<
    HL::FirstFitHeap<
      HL::SizeHeap<
        HL::ZoneHeap<
          HL::PageTableHeap<HL::SourceMmapHeap>,
          DEFAULT_ZONE_SZ> > > >
  MainHeapType;


#endif  // GALLOCY_HEAPLAYERS_HEAPTYPES_H_
