#include <cstring>
#include <cstdlib>

#include "gtest/gtest.h"

#include "diff.h"


TEST(DiffTests, DiffTinyTest) {
  char* align_str1 = NULL;
  char* align_str2 = NULL;
  int ret = diff("GGAATGG", 7, align_str1, "ATG", 3, align_str2);
  ASSERT_EQ(ret, 0);
  ASSERT_STREQ(align_str1, "GGAATGG");
  ASSERT_STREQ(align_str2, "---AT-G");
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
  //print_diff(align_str1, strlen(align_str1),
  //    align_str2, strlen(align_str2));
}
