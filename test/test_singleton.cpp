#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gtest/gtest.h"

#include "libgallocy.h"


TEST(SingletonHeapTests, ZeroMalloc) {
  void* ptr = NULL;
  ptr = singletonHeap.malloc(0);
  ASSERT_NE(ptr, (void*) NULL);
}


TEST(SingletonHeapTests, SimpleMalloc) {
  char* ptr = (char*) singletonHeap.malloc(sizeof(char) * 16);
  ASSERT_NE(ptr, (void *) NULL);
  memset(ptr, 'A', 15);
  ptr[15] = 0;
  ASSERT_EQ(strcmp(ptr, "AAAAAAAAAAAAAAA"), 0);
  singletonHeap.free(ptr);
}


TEST(SingletonHeapTests, SmallMalloc) {
  char* ptr = (char*) singletonHeap.malloc(1);
  ASSERT_TRUE(ptr != NULL);
  ptr[0] = 'A';
  ASSERT_EQ(*ptr, 'A');
}


TEST(SingletonHeapTests, MediumMalloc) {
  int sz = 4312;
  char* ptr = (char*) singletonHeap.malloc(sz);
  ASSERT_TRUE(ptr != NULL);
  for (int i = 0; i < sz; i++)
    ptr[i] = 33 + (i % 126 - 33);
  for (int i = 0; i < sz; i++)
    ASSERT_EQ(ptr[i], 33 + (i % 126 - 33));
  singletonHeap.free(ptr);
}


TEST(SingletonHeapTests, BigMalloc) {
  int sz =  4096 * 16;
  char* ptr = (char*) singletonHeap.malloc(sz);
  ASSERT_TRUE(ptr != NULL);
  for (int i = 0; i < sz; i++)
    ptr[i] = 33 + (i % 126 - 33);
  for (int i = 0; i < sz; i++)
    ASSERT_EQ(ptr[i], 33 + (i % 126 - 33));
  singletonHeap.free(ptr);
}


TEST(SingletonHeapTests, ManyMalloc) {
  char* ptr;
  for (int i = 0; i < 4096; i++) {
    ptr = (char*) singletonHeap.malloc(32);
    ASSERT_TRUE(ptr != NULL);
    for (int j = 0; j < 32; j++)
      ptr[j] = 'A';
    for (int j = 0; j < 32; j++)
      ASSERT_EQ(ptr[j], 'A');
    singletonHeap.free(ptr);
  }
}


TEST(SingletonHeapTests, ReuseAllocation) {
  char* ptr1 = NULL;
  char* ptr2 = NULL;

  ptr1 = (char*) singletonHeap.malloc(128);
  memset(ptr1, 'A', 64);
  singletonHeap.free(ptr1);

  ptr2 = (char*) singletonHeap.malloc(16);
  memset(ptr2, 'B', 16);
  ASSERT_EQ(ptr1, ptr2);
}
