#include "gtest/gtest.h"

#include "myhashmap.h"
#include "spinlock.h"
#include "lockedheap.h"
#include "freelistheap.h"
#include "firstfitheap.h"
#include "zoneheap.h"
#include "source.h"


class MyHeap:
  public HL::LockedHeap<HL::SpinLockType, HL::FreelistHeap<HL::ZoneHeap<SimpleHeap, 16384 - 16> > > {};

typedef HL::MyHashMap<void*, size_t, MyHeap> mapType;


TEST(InternalTest, MyHashMap) {
  void* ptr1 = malloc(8);
  void* ptr2 = malloc(16);
  void* ptr3 = malloc(32);
  ASSERT_TRUE(ptr1 != NULL);
  ASSERT_TRUE(ptr2 != NULL);
  ASSERT_TRUE(ptr3 != NULL);
  mapType map;
  map.set(ptr1, 8);
  map.set(ptr2, 16);
  map.set(ptr3, 32);
  ASSERT_EQ(map.get(ptr1), 8);
  ASSERT_EQ(map.get(ptr2), 16);
  ASSERT_EQ(map.get(ptr3), 32);
  map.erase(ptr1);
  map.erase(ptr2);
  map.erase(ptr3);
  ASSERT_EQ(map.get(ptr1), NULL);
  ASSERT_EQ(map.get(ptr2), NULL);
  ASSERT_EQ(map.get(ptr3), NULL);
}
