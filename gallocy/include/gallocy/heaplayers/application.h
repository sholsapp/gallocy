#ifndef GALLOCY_HEAPLAYERS_APPLICATION_H_
#define GALLOCY_HEAPLAYERS_APPLICATION_H_

// NOTE: Order matters because forward declarations do not exist.
#include "heaplayers/firstfitheap.h"
#include "heaplayers/lockedheap.h"
#include "heaplayers/pagetableheap.h"
#include "heaplayers/sizeheap.h"
#include "heaplayers/source.h"
#include "heaplayers/spinlock.h"
#include "heaplayers/stdlibheap.h"
#include "heaplayers/zoneheap.h"

// TODO(sholsapp): FIX ME: 16 = size of ZoneHeap header.
#define DEFAULT_ZONE_SZ 16384 - 16

/**
 * Shared application memory.
 */
typedef
  HL::StdlibHeap<
  HL::LockedHeap<
  HL::SpinLockType,
    HL::FirstFitHeap<
      HL::SizeHeap<
        HL::ZoneHeap<
          HL::SourceMmapHeap<PURPOSE_APPLICATION_HEAP>,
          DEFAULT_ZONE_SZ> > > > >
  ApplicationHeapType;

#endif  // GALLOCY_HEAPLAYERS_APPLICATION_H_
