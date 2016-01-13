#ifndef GALLOCY_HEAPLAYERS_HEAPTYPES_H_
#define GALLOCY_HEAPLAYERS_HEAPTYPES_H_


// TODO(sholsapp): FIX ME: 16 = size of ZoneHeap header.
#define DEFAULT_ZONE_SZ 16384 - 16


// NOTE: Order matters because forward declarations do not exist.
#include "heaplayers/firstfitheap.h"
#include "heaplayers/debugheap.h"
#include "heaplayers/pagetableheap.h"
#include "heaplayers/sizeheap.h"
#include "heaplayers/zoneheap.h"
#include "heaplayers/source.h"


/**
 * Shared application memory.
 */
typedef
  HL::DebugHeap<
    HL::FirstFitHeap<
      HL::SizeHeap<
        HL::ZoneHeap<
          HL::PageTableHeap<HL::SourceMmapHeap<PURPOSE_APPLICATION_HEAP> >,
          DEFAULT_ZONE_SZ> > > >
  MainHeapType;


#endif  // GALLOCY_HEAPLAYERS_HEAPTYPES_H_
