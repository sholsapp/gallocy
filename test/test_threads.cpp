#include <cstring>
#include <cstdlib>

#include "gtest/gtest.h"

#include "libgallocy.h"
#include "threads.h"


TEST(ThreadsTests, PageAlignmentTest) {
  ASSERT_EQ((long) page_align_ptr((void*) 0x7fff68e07730), (long) 0x7fff68e07000);
}


TEST(ThreadsTests, ThreadsStackAllocationTest) {
  int stack_size = 16;
  void* stack = allocate_thread_stack(NULL, stack_size);
  ASSERT_NE(stack, (void*) NULL);
  ASSERT_EQ(page_align_ptr(stack), stack);
  memset(stack, 'A', 4096 * stack_size);
}
