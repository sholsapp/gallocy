#include "gtest/gtest.h"

#include "frozen.h"


TEST(JsonTests, SimpleJson) {
  static const char *json = "{foo:1,bar:2}";
  char buf[16] = {0};
  struct json_token *arr, *tok;
  arr = parse_json2(json, strlen(json));
  tok = find_json_token(arr, "foo");
  memcpy(buf, tok->ptr, tok->len);
  ASSERT_STREQ(buf, "1");
  tok = find_json_token(arr, "bar");
  memcpy(buf, tok->ptr, tok->len);
  ASSERT_STREQ(buf, "2");
  // XXX: This memory is leaked but it is because the singletonHeap
  // allocator allocated the memory in the json library.
  //free(arr);
}
