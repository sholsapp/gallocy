#include <cstring>
#include <cstdlib>

#include "gtest/gtest.h"

#include "gallocy/allocators/internal.h"
#include "gallocy/utils/diff.h"


TEST(DiffTests, DiffTinyTest) {
  char* align_str1 = NULL;
  char* align_str2 = NULL;
  int ret = diff("GGAATGG", 7, align_str1, "ATG", 3, align_str2);
  ASSERT_EQ(ret, 0);
  ASSERT_STREQ(align_str1, "GGAATGG");
  ASSERT_STREQ(align_str2, "---AT-G");
  // TODO: decide on diff format before testing
  //print_diff(align_str1, strlen(align_str1),
  //    align_str2, strlen(align_str2));
}


TEST(DiffTests, DiffGeneral_1) {
  const char* str1 = "FOO BOP BOOP";
  const char* str2 = "FOOO BOOP BOP";
  char* align_str1 = NULL;
  char* align_str2 = NULL;
  int ret = diff(str1, strlen(str1), align_str1, str2, strlen(str2), align_str2);
  ASSERT_EQ(ret, 0);
  ASSERT_STREQ(align_str1, "F-OO B-OP BOOP");
  ASSERT_STREQ(align_str2, "FOOO BOOP B-OP");
  // TODO: decide on diff format before testing
  //print_diff(align_str1, strlen(align_str1),
  //    align_str2, strlen(align_str2));
}


TEST(DiffTests, DiffGeneral_2) {
  // NOTE(sholsapp): at 1024 this raises a SIGSEGV, presumably because we run
  // out of memory. Hypothesis was tested by increasing available memory, which
  // remedies the situation.
  int mem_sz = 512;
  char* str1 = (char*) internal_malloc(sizeof(char) * mem_sz);
  char* str2 = (char*) internal_malloc(sizeof(char) * mem_sz);
  char* str1align = NULL;
  char* str2align = NULL;
  for (int i = 0; i < mem_sz; i++) {
    str1[i] = rand() % 255;
  }
  memcpy(str2, str1, mem_sz);
  for (int i = 0; i < mem_sz; i++) {
    if (rand() % 10 == 1)
      str2[i] = rand() % 255;
  }
  int ret = diff(str1, mem_sz, str1align, str2, mem_sz, str2align);
  ASSERT_EQ(ret, 0);
}
