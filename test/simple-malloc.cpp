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
