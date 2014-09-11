#include <cstring>
#include <cstdlib>

#include "gtest/gtest.h"

#include "diff.h"


TEST(DiffTests, DiffTinyTest) {
  diff("GGAATGG", 7, "ATG", 3);
}


TEST(DiffTests, DiffDeletes) {
  const char* str1 = "XAAAAAAAAAAAAAAAXXAAAAAAAAAAAAAAAAAAA";
  const char* str2 = "XCCAAAAAAAAABBAAAAAAAA";
  diff(str1, strlen(str1), str2, strlen(str2));
}


TEST(DiffTest, DiffAdditions) {
  const char* str1 = "XCCAAAAAAAAABBAAAAAAAA";
  const char* str2 = "XAAAAAAAAAAAAAAAXXAAAAAAAAAAAAAAAAAAA";
  diff(str1, strlen(str1), str2, strlen(str2));
}


TEST(DiffTest, DiffGeneral_1) {
  const char* str1 = "FOO BAR BAZ BANG BOING BOP BOOP";
  const char* str2 = "FOOO BAR BAZ BANG BING BOOP BOP";
  diff(str1, strlen(str1), str2, strlen(str2));
}
