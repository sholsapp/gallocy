#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gtest/gtest.h"

#include "libgallocy.h"


TEST(FreeTests, NullFree) {
  custom_free(NULL);
}


TEST(FreeTests, SimpleFree) {
  char* ptr1 = (char*) custom_malloc(sizeof(char) * 16);
  ASSERT_TRUE(ptr1 != NULL);
  char* ptr2 = (char*) custom_malloc(sizeof(char) * 16);
  ASSERT_TRUE(ptr2 != NULL);
  custom_free(ptr1);
  custom_free(ptr2);
  char* ptr3 = (char*) custom_malloc(sizeof(char) * 16);
  ASSERT_TRUE(ptr3 != NULL);
  char* ptr4 = (char*) custom_malloc(sizeof(char) * 16);
  ASSERT_TRUE(ptr4 != NULL);
  custom_free(ptr3);
  custom_free(ptr4);
}


TEST(FreeTests, UsageFree) {
  char* ptr1 = (char*) custom_malloc(sizeof(char) * 32);
  ASSERT_TRUE(ptr1 != NULL);
  for (int i = 0; i < 32; i++)
    ptr1[i] = 'A';
  custom_free(ptr1);
  char* ptr2 = (char*) custom_malloc(sizeof(char) * 16);
  ASSERT_TRUE(ptr2 != NULL);
  for (int i = 0; i < 16; i++)
    ptr2[i] = 'B';
  custom_free(ptr2);
}


TEST(FreeTests, CheckManySmallFrees) {

  const size_t alloc_sz = 239;
  const size_t arr_sz = 4096;
  char* small_ptrs[arr_sz];

  // Allocate a bunch of pointers and fill them with ascii
  for (uint64_t i = 0; i < arr_sz; i++) {
    small_ptrs[i] = (char*) custom_malloc(sizeof(char) * alloc_sz);
    ASSERT_NE(small_ptrs[i], (void*) NULL);
    memset(&small_ptrs[i][0], (char) i % 255, alloc_sz);
  }

  // Free the even half of the pointers
  for (uint64_t i = 0; i < arr_sz; i += 2) {
    custom_free(small_ptrs[i]);
  }

  // Allocatote a bunch of pointers of the same size and fill with null byte
  for (uint64_t i = 0; i < arr_sz / 2; i++) {
    char* trash = (char*) custom_malloc(sizeof(char) * alloc_sz);
    ASSERT_NE(trash, (void*) NULL) << "Failed to allocate trash pointer on iteration [" << i << "].";
    memset(trash, 0, alloc_sz);
  }

  // Check for any clobbered memory
  for (uint64_t i = 1; i < arr_sz; i += 2) {
    ASSERT_NE(small_ptrs[i], (void*) NULL);
    for (uint64_t j = 0; j < alloc_sz; j++) {
      char target = (char) i % 255;
      ASSERT_EQ(small_ptrs[i][j], target) << "Failed at iteration [" << i << "] at offset [" << j << "].";
    }
  }

  // Clean up the pointers
  for (uint64_t i = 0; i < arr_sz; i++) {
    custom_free(small_ptrs[i]);
  }

}
