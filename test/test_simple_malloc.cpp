#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gtest/gtest.h"

#include "libgallocy.h"


class MallocTests: public ::testing::Test {
  protected:
    virtual void TearDown() {
      __reset_memory_allocator();
    }
};


TEST_F(MallocTests, ZeroMalloc) {
  void* ptr = NULL;
  ptr = custom_malloc(0);
  ASSERT_NE(ptr, (void*) NULL);
  ASSERT_EQ(custom_malloc_usable_size(ptr), 0);
}


TEST_F(MallocTests, SimpleMalloc) {
  char* ptr = (char*) custom_malloc(sizeof(char) * 16);
  ASSERT_NE(ptr, (void *) NULL);
  ASSERT_EQ(custom_malloc_usable_size(ptr), 16);
  memset(ptr, 'A', 15);
  ptr[15] = 0;
  ASSERT_EQ(strcmp(ptr, "AAAAAAAAAAAAAAA"), 0);
  custom_free(ptr);
}


TEST_F(MallocTests, SmallMalloc) {
  char* ptr = (char*) custom_malloc(1);
  ASSERT_TRUE(ptr != NULL);
  ASSERT_EQ(custom_malloc_usable_size(ptr), 1);
  ptr[0] = 'A';
  ASSERT_EQ(*ptr, 'A');
}


TEST_F(MallocTests, MediumMalloc) {
  int sz = 4312;
  char* ptr = (char*) custom_malloc(sz);
  ASSERT_TRUE(ptr != NULL);
  for (int i = 0; i < sz; i++)
    ptr[i] = 33 + (i % 126 - 33);
  for (int i = 0; i < sz; i++)
    ASSERT_EQ(ptr[i], 33 + (i % 126 - 33));
  custom_free(ptr);
}


TEST_F(MallocTests, BigMalloc) {
  int sz =  4096 * 16;
  char* ptr = (char*) custom_malloc(sz);
  ASSERT_TRUE(ptr != NULL);
  for (int i = 0; i < sz; i++)
    ptr[i] = 33 + (i % 126 - 33);
  for (int i = 0; i < sz; i++)
    ASSERT_EQ(ptr[i], 33 + (i % 126 - 33));
  custom_free(ptr);
}


TEST_F(MallocTests, ManyMalloc) {
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


TEST_F(MallocTests, ReuseAllocation) {
  char* ptr1 = NULL;
  char* ptr2 = NULL;

  ptr1 = (char*) custom_malloc(128);
  memset(ptr1, 'A', 64);
  custom_free(ptr1);

  ptr2 = (char*) custom_malloc(16);
  memset(ptr2, 'B', 16);
  ASSERT_EQ(ptr1, ptr2);
}


TEST_F(MallocTests, ReuseOldAllocations) {
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

  ASSERT_NE(ptr, _ptr);
  ASSERT_EQ(custom_malloc_usable_size(ptr), 156);

  custom_free(ptr);
}


TEST_F(MallocTests, ManyAllocations) {
  int MANY = 1000;
  char* ptr = NULL;
  for (int i = 0; i < MANY; i++) {
    ptr = (char*) custom_malloc(256);
    ASSERT_TRUE(ptr != NULL);
    memset(ptr, 'A', 256);
    custom_free(ptr);
  }
}


TEST_F(MallocTests, RandomAllocations) {
  int rand_sz = 0;
  void* ptr = NULL;
  for (int i = 0; i < 4096; i++) {
    int sz = rand() % 4096;
    ptr = custom_malloc(sz);
    ASSERT_NE(ptr, (void*) NULL);
    ASSERT_GE(custom_malloc_usable_size(ptr), sz);
    custom_free(ptr);
  }
}


TEST_F(MallocTests, ManyReallocs) {
  char* ptr = NULL;
  char* new_ptr = NULL;
  size_t sz = 16;
  size_t max_sz = 1024;
  ptr = (char*) custom_malloc(sizeof(char) * 16);
  memset(ptr, 'A', 16);
  for (int i = 1; i <= max_sz - sz; i++) {
    new_ptr = (char*) custom_realloc(ptr, sz + i);
    ASSERT_NE(new_ptr, (void*) NULL);
    memset(new_ptr, 'A', sz + i);
    ptr = new_ptr;
  }
  ASSERT_EQ(custom_malloc_usable_size(ptr), max_sz);
  custom_free(ptr);
}
