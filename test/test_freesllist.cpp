#include "gtest/gtest.h"

#include "freesllist.h"


TEST(InternalTest, FreeSLList) {
  const int sz = 2;
  // Setup some pointers to memory
  void* ptr[sz] = {};
  for (int i = 0; i < sz; i++)
    ptr[i] = malloc(16);
  // Verify the pointers are all different
  for (int i = 0; i < sz; i++ ) {
    for (int j = 0; j < sz; j++) {
      if (i != j) {
        ASSERT_NE(ptr[i], ptr[j]);
      }
    }
  }
  // Test FreeSLList
  FreeSLList freelist;
  freelist.insert(ptr[0]);
  ASSERT_EQ(freelist.get(), ptr[0]);
  freelist.insert(ptr[1]);
  ASSERT_EQ(freelist.get(), ptr[1]);
  ASSERT_TRUE(freelist.get() == NULL);
  // Clean up pointers to memory
  for (int i = 0; i < sz; i++)
    free(ptr[i]);
}
