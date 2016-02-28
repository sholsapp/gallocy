#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <thread>
#include <vector>

#include "gtest/gtest.h"

#include "libgallocy.h"


class InternalAllocatorTests: public ::testing::Test {
  protected:
    virtual void TearDown() {
      __reset_memory_allocator();
    }
};


TEST_F(InternalAllocatorTests, ZeroMalloc) {
  void* ptr = NULL;
  ptr = internal_malloc(0);
  ASSERT_NE(ptr, (void*) NULL);
  ASSERT_GE(internal_malloc_usable_size(ptr), static_cast<size_t>(0));
}


TEST_F(InternalAllocatorTests, ZeroRealloc) {
  void *ptr = NULL;
  ptr = internal_realloc(NULL, 0);
  ASSERT_NE(ptr, (void *) NULL);
  ASSERT_GE(internal_malloc_usable_size(ptr), static_cast<size_t>(0));
}


TEST_F(InternalAllocatorTests, ZeroCalloc) {
  void *ptr = NULL;
  ptr = calloc(0, 0);
  ASSERT_NE(ptr, (void *) NULL);
  ASSERT_GE(internal_malloc_usable_size(ptr), static_cast<size_t>(0));
}


TEST_F(InternalAllocatorTests, SimpleMalloc) {
  char* ptr = (char*) internal_malloc(sizeof(char) * 16);
  ASSERT_NE(ptr, (void *) NULL);
  ASSERT_EQ(internal_malloc_usable_size(ptr), static_cast<size_t>(16));
  memset(ptr, 'A', 15);
  ptr[15] = 0;
  ASSERT_EQ(strcmp(ptr, "AAAAAAAAAAAAAAA"), 0);
  internal_free(ptr);
}


TEST_F(InternalAllocatorTests, SmallMalloc) {
  char* ptr = (char*) internal_malloc(1);
  ASSERT_TRUE(ptr != NULL);
  ASSERT_EQ(internal_malloc_usable_size(ptr), static_cast<size_t>(sizeof(size_t) * 2));
  ptr[0] = 'A';
  ASSERT_EQ(*ptr, 'A');
}


TEST_F(InternalAllocatorTests, MediumMalloc) {
  int sz = 4312;
  char* ptr = (char*) internal_malloc(sz);
  ASSERT_TRUE(ptr != NULL);
  for (int i = 0; i < sz; i++)
    ptr[i] = 33 + (i % 126 - 33);
  for (int i = 0; i < sz; i++)
    ASSERT_EQ(ptr[i], 33 + (i % 126 - 33));
  internal_free(ptr);
}


TEST_F(InternalAllocatorTests, BigMalloc) {
  // A size allocated by the Python interpreter during compilation of Python
  // 3.3 that causes weird deadlock
  int sz =  91424;
  char* ptr = (char*) internal_malloc(sz);
  ASSERT_TRUE(ptr != NULL);
  for (int i = 0; i < sz; i++)
    ptr[i] = 33 + (i % 126 - 33);
  for (int i = 0; i < sz; i++)
    ASSERT_EQ(ptr[i], 33 + (i % 126 - 33));
  internal_free(ptr);
}


TEST_F(InternalAllocatorTests, ManyMalloc) {
  char* ptr;
  for (int i = 0; i < 4096; i++) {
    ptr = (char*) internal_malloc(32);
    ASSERT_TRUE(ptr != NULL);
    for (int j = 0; j < 32; j++)
      ptr[j] = 'A';
    for (int j = 0; j < 32; j++)
      ASSERT_EQ(ptr[j], 'A');
    internal_free(ptr);
  }
}


TEST_F(InternalAllocatorTests, ReuseAllocation) {
  char* ptr1 = NULL;
  char* ptr2 = NULL;

  ptr1 = (char*) internal_malloc(128);
  memset(ptr1, 'A', 64);
  internal_free(ptr1);

  ptr2 = (char*) internal_malloc(16);
  memset(ptr2, 'B', 16);
  ASSERT_EQ(ptr1, ptr2);
}


TEST_F(InternalAllocatorTests, ReuseOldAllocations) {
  char* ptr;
  char* _ptr = NULL;
  for (int i = 0; i < 8; i++) {
    ptr = (char*) internal_malloc(64);
    ASSERT_TRUE(ptr != NULL);
    if (_ptr)
      ASSERT_EQ(_ptr, ptr) << "Failure on iteration [" << i << "]";
    memset(ptr, 'A', 64);
    internal_free(ptr);
    _ptr = ptr;
  }
  ptr = (char*) internal_malloc(156);
  ASSERT_TRUE(ptr != NULL);

  ASSERT_NE(ptr, _ptr);
  ASSERT_GE(internal_malloc_usable_size(ptr), static_cast<size_t>(156));

  internal_free(ptr);
}


TEST_F(InternalAllocatorTests, ManyAllocations) {
  int MANY = 1000;
  char* ptr = NULL;
  for (int i = 0; i < MANY; i++) {
    ptr = (char*) internal_malloc(256);
    ASSERT_TRUE(ptr != NULL);
    memset(ptr, 'A', 256);
    internal_free(ptr);
  }
}


TEST_F(InternalAllocatorTests, RandomAllocations) {
  void* ptr = NULL;
  for (int i = 0; i < 4096; i++) {
    int sz = rand() % 4096;
    ptr = internal_malloc(sz);
    ASSERT_NE(ptr, (void*) NULL);
    ASSERT_GE(internal_malloc_usable_size(ptr), static_cast<size_t>(sz));
    internal_free(ptr);
  }
}


TEST_F(InternalAllocatorTests, ManyReallocs) {
  char* ptr = NULL;
  char* new_ptr = NULL;
  size_t sz = 16;
  size_t max_sz = 1024;
  ptr = (char*) internal_malloc(sizeof(char) * 16);
  memset(ptr, 'A', 16);
  for (uint64_t i = 1; i <= max_sz - sz; i++) {
    new_ptr = (char*) internal_realloc(ptr, sz + i);
    ASSERT_NE(new_ptr, (void*) NULL);
    memset(new_ptr, 'A', sz + i);
    ptr = new_ptr;
  }
  ASSERT_EQ(internal_malloc_usable_size(ptr), max_sz);
  internal_free(ptr);
}


TEST_F(InternalAllocatorTests, CheckManySmallAllocations) {
  const size_t alloc_sz = 256;
  const size_t arr_sz = 4096;
  char* small_ptrs[arr_sz];

  for (uint64_t i = 0; i < arr_sz; i++) {
    small_ptrs[i] = (char*) internal_malloc(sizeof(char) * alloc_sz);
    ASSERT_NE(small_ptrs[i], (void*) NULL);
    memset(&small_ptrs[i][0], (char) i % 255, alloc_sz);
  }

  for (uint64_t i = 0; i < arr_sz; i++) {
    ASSERT_NE(small_ptrs[i], (void*) NULL);
    for (uint64_t j = 0; j < alloc_sz; j++) {
      char target = (char) i % 255;
      ASSERT_EQ(small_ptrs[i][j], target) << "Failed at iteration [" << i << "] at offset [" << j << "].";
    }
  }

  for (uint64_t i = 0; i < arr_sz; i++) {
    internal_free(small_ptrs[i]);
  }

}


TEST_F(InternalAllocatorTests, CheckManyRandomAllocations) {

  const int rand_sz = 4096 * 1;
  const size_t arr_sz = 256;
  char* small_ptrs[arr_sz];
  size_t small_ptrs_sz[arr_sz];

  for (uint64_t i = 0; i < arr_sz; i++) {
    small_ptrs_sz[i] = rand() % rand_sz;
    small_ptrs[i] = (char*) internal_malloc(sizeof(char) * small_ptrs_sz[i]);
    ASSERT_NE(small_ptrs[i], (void*) NULL);
    memset(&small_ptrs[i][0], (char) i % 255, small_ptrs_sz[i]);
  }

  for (uint64_t i = 0; i < arr_sz; i++) {
    ASSERT_NE(small_ptrs[i], (void*) NULL);
    for (uint64_t j = 0; j < small_ptrs_sz[i]; j++) {
      char target = (char) i % 255;
      ASSERT_EQ(small_ptrs[i][j], target) << "Failed at iteration [" << i << "] at offset [" << j << "].";
    }
  }

  for (uint64_t i = 0; i < arr_sz; i++) {
    internal_free(small_ptrs[i]);
  }

}


TEST_F(InternalAllocatorTests, LeakCheck) {
  char* low = (char *) internal_malloc(1);
  char* high = low;
  internal_free(low);
  char* p, *q, *r;
  for(int i = 0; i < 10000; i++){
    p = (char *) internal_malloc(4096);
    q = (char *) internal_malloc(4096 * 2 + 1);
    r = (char *) internal_malloc(1);
    if (p < low)
      low = p;
    if (q < low)
      low = q;
    if (r < low)
      low = r;
    if (p > high)
      high = p;
    if (q > high)
      high = q;
    if (r > high)
      high = r;
    internal_free(p);
    internal_free(q);
    internal_free(r);
  }
  ASSERT_TRUE((uint64_t) high - (uint64_t) low < 4096 * 2);
}


void internal_parallel_test_work() {
  const int rand_sz = 4096 * 1;
  const size_t arr_sz = 256;
  char* small_ptrs[arr_sz];
  size_t small_ptrs_sz[arr_sz];

  for (uint64_t i = 0; i < arr_sz; i++) {
    small_ptrs_sz[i] = rand() % rand_sz;
    small_ptrs[i] = (char*) internal_malloc(sizeof(char) * small_ptrs_sz[i]);
    ASSERT_NE(small_ptrs[i], (void*) NULL);
    memset(&small_ptrs[i][0], (char) i % 255, small_ptrs_sz[i]);
  }

  for (uint64_t i = 0; i < arr_sz; i++) {
    ASSERT_NE(small_ptrs[i], (void*) NULL);
    for (uint64_t j = 0; j < small_ptrs_sz[i]; j++) {
      char target = (char) i % 255;
      ASSERT_EQ(small_ptrs[i][j], target) << "Failed at iteration [" << i << "] at offset [" << j << "].";
    }
  }

  for (uint64_t i = 0; i < arr_sz; i++) {
    internal_free(small_ptrs[i]);
  }

  return;
}

TEST_F(InternalAllocatorTests, ParallelCheck) {
  std::vector<std::thread> threads;
  for (int i = 0; i < 32; i++) {
    threads.push_back(std::thread(internal_parallel_test_work));
  }
  for (auto &thread : threads) {
    thread.join();
  }
}


TEST_F(InternalAllocatorTests, GrowingRealloc) {
  void *ptr = NULL;
  size_t sz = 16;
  for (uint64_t i = 0; i < 512; i++) {
    ptr = internal_realloc(ptr, sz * i);
    ASSERT_NE(ptr, (void *) NULL);
    memset(ptr, 0, sz * i);
  }
}


TEST_F(InternalAllocatorTests, SimpleCalloc) {
  void *ptr = NULL;
  ptr = internal_calloc(1, 16);
  ASSERT_NE(ptr, (void *) NULL);
}


TEST_F(InternalAllocatorTests, SimpleStrdup) {
  char *from = reinterpret_cast<char *>(internal_malloc(sizeof(char) * 16));
  memset(from, 'A', 15);
  from[15] = 0;
  char *to = internal_strdup(from);
  ASSERT_NE(from, to);
  ASSERT_EQ(strcmp(from, to), 0);
}
