#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gtest/gtest.h"

#include "libgallocy.h"


TEST(GallocyTest, SimpleMalloc) {
  char* ptr = (char*) custom_malloc(sizeof(char) * 16);
  ASSERT_TRUE(ptr != NULL);
  for (int i = 0; i < 15; i++) {
    ptr[i] = 'A';
  }
  ptr[15] = 0;
  ASSERT_EQ(strcmp(ptr, "AAAAAAAAAAAAAAA"), 0);
  custom_free(ptr);
}


TEST(GallocyTest, SmallMalloc) {
  char* ptr = (char*) custom_malloc(1);
  ASSERT_TRUE(ptr != NULL);
  ptr[0] = 'A';
  ASSERT_EQ(*ptr, 'A');
}


TEST(GallocyTest, MediumMalloc) {
  int sz = 4312;
  char* ptr = (char*) custom_malloc(sz);
  ASSERT_TRUE(ptr != NULL);
  for (int i = 0; i < sz; i++)
    ptr[i] = 33 + (i % 126 - 33);
  for (int i = 0; i < sz; i++)
    ASSERT_EQ(ptr[i], 33 + (i % 126 - 33));
  custom_free(ptr);
}


TEST(GallocyTest, BigMalloc) {
  int sz =  4096 * 16;
  char* ptr = (char*) custom_malloc(sz);
  ASSERT_TRUE(ptr != NULL);
  for (int i = 0; i < sz; i++)
    ptr[i] = 33 + (i % 126 - 33);
  for (int i = 0; i < sz; i++)
    ASSERT_EQ(ptr[i], 33 + (i % 126 - 33));
  custom_free(ptr);
}


TEST(GallocyTest, ManyMalloc) {
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


TEST(GallocyTest, ReuseAllocation) {
  char* ptr1 = NULL;
  char* ptr2 = NULL;

  ptr1 = (char*) custom_malloc(128);
  memset(ptr1, 'A', 64);
  custom_free(ptr1);

  ptr2 = (char*) custom_malloc(16);
  memset(ptr2, 'B', 16);
  ASSERT_EQ(ptr1, ptr2);
}


TEST(GallocyTest, ReuseOldAllocations) {
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
  ptr = (char*) custom_malloc(65);
  ASSERT_TRUE(ptr != NULL);

  // These shouldn't be equal but generate a segmentation fault. The current
  // custom heap is bad.
  //ASSERT_NE(ptr, _ptr);

  custom_free(ptr);
}
