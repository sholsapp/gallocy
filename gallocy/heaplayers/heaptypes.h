#ifndef _HEAPTYPES_H
#define _HEAPTYPES_H


#include "heaplayers/debugheap.h"
#include "heaplayers/pagetableheap.h"
#include "heaplayers/source.h"
#include "heaplayers/addheap.h"
#include "heaplayers/sizeheap.h"
#include "heaplayers/myhashmap.h"
#include "heaplayers/spinlock.h"
#include "heaplayers/lockedheap.h"
#include "heaplayers/freelistheap.h"
#include "heaplayers/firstfitheap.h"
#include "heaplayers/zoneheap.h"
#include "heaplayers/stl.h"



// FIX ME: 16 = size of ZoneHeap header.
#define DEFAULT_ZONE_SZ 16384 - 16


// NOTE: there are two other heap definitions in heaplayers/source.h which are
// used for internal purposes for the heaps there.


typedef
  HL::DebugHeap<
    HL::FirstFitHeap<
      HL::SizeHeap<
        HL::ZoneHeap<
          HL::PageTableHeap<SourceHeap>,
          DEFAULT_ZONE_SZ> > > >
  MainHeapType;


typedef
  HL::LockedHeap<
    HL::SpinLockType,
    HL::FirstFitHeap<
      HL::SizeHeap<
        HL::ZoneHeap<
          SingletonHeap,
          DEFAULT_ZONE_SZ> > > >
  SqlitexSizeHeapType;


typedef
  HL::FirstFitHeap<
    HL::SizeHeap<
      HL::ZoneHeap<
        SourceHeap,
        DEFAULT_ZONE_SZ> > >
  SqliteAllocatorHeapType;


#endif
