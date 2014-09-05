#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gtest/gtest.h"

#include "libgallocy.h"


TEST(MallocTests, ZeroMalloc) {
  void* ptr = NULL;
  ptr = custom_malloc(0);
  ASSERT_NE(ptr, (void*) NULL);
}


TEST(MallocTests, SimpleMalloc) {
  char* ptr = (char*) custom_malloc(sizeof(char) * 16);
  ASSERT_TRUE(ptr != NULL);
  for (int i = 0; i < 15; i++) {
    ptr[i] = 'A';
  }
  ptr[15] = 0;
  ASSERT_EQ(strcmp(ptr, "AAAAAAAAAAAAAAA"), 0);
  custom_free(ptr);
}


TEST(MallocTests, SmallMalloc) {
  char* ptr = (char*) custom_malloc(1);
  ASSERT_TRUE(ptr != NULL);
  ptr[0] = 'A';
  ASSERT_EQ(*ptr, 'A');
}


TEST(MallocTests, MediumMalloc) {
  int sz = 4312;
  char* ptr = (char*) custom_malloc(sz);
  ASSERT_TRUE(ptr != NULL);
  for (int i = 0; i < sz; i++)
    ptr[i] = 33 + (i % 126 - 33);
  for (int i = 0; i < sz; i++)
    ASSERT_EQ(ptr[i], 33 + (i % 126 - 33));
  custom_free(ptr);
}


TEST(MallocTests, BigMalloc) {
  int sz =  4096 * 16;
  char* ptr = (char*) custom_malloc(sz);
  ASSERT_TRUE(ptr != NULL);
  for (int i = 0; i < sz; i++)
    ptr[i] = 33 + (i % 126 - 33);
  for (int i = 0; i < sz; i++)
    ASSERT_EQ(ptr[i], 33 + (i % 126 - 33));
  custom_free(ptr);
}


TEST(MallocTests, ManyMalloc) {
  char* ptr;
  for (int i = 0; i < 4096; i++) {
    ptr = (char*) custom_malloc(32);
    ASSERT_TRUE(ptr != NULL);
    for (int j = 0; j < 32; j++)
      ptr[j] = 'A';
    for (int j = 0; j < 32; j++)
      ASSERT_EQ(ptr[j], 'A');
    custom_free(ptr);
  }
}


TEST(MallocTests, ReuseAllocation) {
  char* ptr1 = NULL;
  char* ptr2 = NULL;

  ptr1 = (char*) custom_malloc(128);
  memset(ptr1, 'A', 64);
  custom_free(ptr1);

  ptr2 = (char*) custom_malloc(16);
  memset(ptr2, 'B', 16);
  ASSERT_EQ(ptr1, ptr2);
}


TEST(MallocTests, ReuseOldAllocations) {
  char* ptr;
  char* _ptr = NULL;
  for (int i = 0; i < 8; i++) {
    ptr = (char*) custom_malloc(64);
    ASSERT_TRUE(ptr != NULL);
    if (_ptr)
      ASSERT_EQ(_ptr, ptr) << "Failure on iteration [" << i << "]";
    memset(ptr, 'A', 64);
    custom_free(ptr);
    _ptr = ptr;
  }
  ptr = (char*) custom_malloc(156);
  ASSERT_TRUE(ptr != NULL);

  // TODO: fix me -- need test case isolation.
  //ASSERT_NE(ptr, _ptr);

  custom_free(ptr);
}


TEST(MallocTests, ManyAllocations) {
  int MANY = 1000;
  char* ptr = NULL;
  for (int i = 0; i < MANY; i++) {
    ptr = (char*) custom_malloc(256);
    ASSERT_TRUE(ptr != NULL);
    memset(ptr, 'A', 256);
    custom_free(ptr);
  }
}
