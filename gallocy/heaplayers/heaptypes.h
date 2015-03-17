#ifndef _HEAPTYPES_H
#define _HEAPTYPES_H


// FIX ME: 16 = size of ZoneHeap header.
#define DEFAULT_ZONE_SZ 16384 - 16


#include "heaplayers/debugheap.h"
#include "heaplayers/pagetableheap.h"
#include "heaplayers/addheap.h"
#include "heaplayers/sizeheap.h"
#include "heaplayers/myhashmap.h"
#include "heaplayers/spinlock.h"
#include "heaplayers/lockedheap.h"
#include "heaplayers/freelistheap.h"
#include "heaplayers/firstfitheap.h"
#include "heaplayers/zoneheap.h"
#include "heaplayers/stl.h"
#include "heaplayers/stdlibheap.h"


namespace HL {

class SourceMmapHeap;

}


typedef
  HL::StdlibHeap<
    HL::FirstFitHeap<
      HL::SizeHeap<
        HL::ZoneHeap<
          HL::SourceMmapHeap,
          DEFAULT_ZONE_SZ> > > >
  SingletonHeapType;


#include "heaplayers/source.h"


typedef
  HL::DebugHeap<
    HL::FirstFitHeap<
      HL::SizeHeap<
        HL::ZoneHeap<
          HL::PageTableHeap<HL::SourceMmapHeap>,
          DEFAULT_ZONE_SZ> > > >
  MainHeapType;


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
  HL::FirstFitHeap<
    HL::SizeHeap<
      HL::ZoneHeap<
        HL::SourceMmapHeap,
        DEFAULT_ZONE_SZ> > >
  SqliteAllocatorHeapType;


#endif
