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
