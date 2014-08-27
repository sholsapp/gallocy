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


TEST(GallocyTest, UsageFree) {
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
