#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <thread>
#include <vector>

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
  ASSERT_GE(custom_malloc_usable_size(ptr), static_cast<size_t>(0));
}


TEST_F(MallocTests, SimpleMalloc) {
  char* ptr = (char*) custom_malloc(sizeof(char) * 16);
  ASSERT_NE(ptr, (void *) NULL);
  ASSERT_EQ(custom_malloc_usable_size(ptr), static_cast<size_t>(16));
  memset(ptr, 'A', 15);
  ptr[15] = 0;
  ASSERT_EQ(strcmp(ptr, "AAAAAAAAAAAAAAA"), 0);
  custom_free(ptr);
}


TEST_F(MallocTests, SmallMalloc) {
  char* ptr = (char*) custom_malloc(1);
  ASSERT_TRUE(ptr != NULL);
  ASSERT_EQ(custom_malloc_usable_size(ptr), static_cast<size_t>(sizeof(size_t) * 2));
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
  // A size allocated by the Python interpreter during compilation of Python
  // 3.3 that causes weird deadlock
  int sz =  91424;
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
  ASSERT_GE(custom_malloc_usable_size(ptr), static_cast<size_t>(156));

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
  void* ptr = NULL;
  for (int i = 0; i < 4096; i++) {
    int sz = rand() % 4096;
    ptr = custom_malloc(sz);
    ASSERT_NE(ptr, (void*) NULL);
    ASSERT_GE(custom_malloc_usable_size(ptr), static_cast<size_t>(sz));
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
  for (uint64_t i = 1; i <= max_sz - sz; i++) {
    new_ptr = (char*) custom_realloc(ptr, sz + i);
    ASSERT_NE(new_ptr, (void*) NULL);
    memset(new_ptr, 'A', sz + i);
    ptr = new_ptr;
  }
  ASSERT_EQ(custom_malloc_usable_size(ptr), max_sz);
  custom_free(ptr);
}


TEST_F(MallocTests, CheckManySmallAllocations) {

  const size_t alloc_sz = 256;
  const size_t arr_sz = 4096;
  char* small_ptrs[arr_sz];

  for (uint64_t i = 0; i < arr_sz; i++) {
    small_ptrs[i] = (char*) custom_malloc(sizeof(char) * alloc_sz);
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
    custom_free(small_ptrs[i]);
  }

}


TEST_F(MallocTests, CheckManyRandomAllocations) {

  const int rand_sz = 4096 * 1;
  const size_t arr_sz = 256;
  char* small_ptrs[arr_sz];
  size_t small_ptrs_sz[arr_sz];

  for (uint64_t i = 0; i < arr_sz; i++) {
    small_ptrs_sz[i] = rand() % rand_sz;
    small_ptrs[i] = (char*) custom_malloc(sizeof(char) * small_ptrs_sz[i]);
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
    custom_free(small_ptrs[i]);
  }

}


TEST_F(MallocTests, LeakCheck) {
  char* low = (char *) custom_malloc(1);
  char* high = low;
  custom_free(low);
  char* p, *q, *r;
  for(int i = 0; i < 10000; i++){
    p = (char *) custom_malloc(4096);
    q = (char *) custom_malloc(4096 * 2 + 1);
    r = (char *) custom_malloc(1);
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
    custom_free(p);
    custom_free(q);
    custom_free(r);
  }
  ASSERT_TRUE((uint64_t) high - (uint64_t) low < 4096 * 2);
}


void parallel_test_work() {
  const int rand_sz = 4096 * 1;
  const size_t arr_sz = 256;
  char* small_ptrs[arr_sz];
  size_t small_ptrs_sz[arr_sz];

  for (uint64_t i = 0; i < arr_sz; i++) {
    small_ptrs_sz[i] = rand() % rand_sz;
    small_ptrs[i] = (char*) custom_malloc(sizeof(char) * small_ptrs_sz[i]);
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
    custom_free(small_ptrs[i]);
  }

  return;
}

TEST_F(MallocTests, ParallelCheck) {
  std::vector<std::thread> threads;
  for (int i = 0; i < 32; i++) {
    threads.push_back(std::thread(parallel_test_work));
  }
  for (auto &thread : threads) {
    std::cout << "Joining" << std::endl;
    thread.join();
  }
}
