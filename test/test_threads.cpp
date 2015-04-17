#include <cstring>
#include <cstdlib>

#include "gtest/gtest.h"

#include "libgallocy.h"
#include "threads.h"


TEST(ThreadsTests, PageAlignmentTest) {
  ASSERT_EQ((long) page_align_ptr((void*) 0x7fff68e07730), (long) 0x7fff68e07000);
}


TEST(ThreadsTests, MicroStackAllocationTest) {
  int stack_size = 1;
  char* stack = (char*) allocate_thread_stack(NULL, stack_size);
  ASSERT_NE(stack, (void*) NULL);
  ASSERT_EQ(page_align_ptr(stack), stack);
  memset(stack, 'A', PAGE_SZ * stack_size);
}


TEST(ThreadsTests, MediumStackAllocationTest) {
  int stack_size = 16;
  char* stack = (char*) allocate_thread_stack(NULL, stack_size);
  ASSERT_NE(stack, (void*) NULL);
  ASSERT_EQ(page_align_ptr(stack), stack);
  memset(stack, 'A', PAGE_SZ * stack_size);
}


TEST(ThreadsTests, LargeStackAllocationTest) {
  int stack_size = 256;
  char* stack = (char*) allocate_thread_stack(NULL, stack_size);
  ASSERT_NE(stack, (void*) NULL);
  ASSERT_EQ(page_align_ptr(stack), stack);
  memset(stack, 'A', PAGE_SZ * stack_size);
}

TEST(ThreadsTest, HitTopGuardPage) {
  int stack_size = 1;
  char* stack = (char*) allocate_thread_stack(NULL, stack_size);
  ASSERT_NE(stack, (void*) NULL);
  ASSERT_EQ(page_align_ptr(stack), stack);
  ASSERT_DEATH({ memset(stack - 1, 'A', 1); }, ".*");
}

TEST(ThreadsTest, HitBottomGuardPage) {
  int stack_size = 1;
  char* stack = (char*) allocate_thread_stack(NULL, stack_size);
  ASSERT_NE(stack, (void*) NULL);
  ASSERT_EQ(page_align_ptr(stack), stack);
  ASSERT_DEATH({ memset(stack + (PAGE_SZ * stack_size) + 1, 'A', 1); }, ".*");
}
