#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gtest/gtest.h"

#include "libgallocy.h"


TEST(GallocyTest, SimpleFree) {
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
