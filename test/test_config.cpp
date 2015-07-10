#include <cstring>
#include <cstdlib>

#include "gtest/gtest.h"

#include "config.h"


TEST(ConfigTests, ReadFileTest) {
  gallocy::string contents(read_file("test/data/config.json"));
  ASSERT_EQ(contents, "{\"foo\": \"bar\"}\n");
}

TEST(ConfigTests, Something) {
  const int buf_sz = 256;
  gallocy::string contents(read_file("test/data/gallocy-config.json"));
  char buf[buf_sz] = {0};
  struct json_token *arr, *tok;
  arr = parse_json2(contents.c_str(), strlen(contents.c_str()));
  tok = find_json_token(arr, "me");
  memcpy(buf, tok->ptr, tok->len);
  ASSERT_STREQ(buf, "http://0.0.0.0:8080");
  memset(buf, 0, buf_sz);
  tok = find_json_token(arr, "peer[0]");
  memcpy(buf, tok->ptr, tok->len);
  ASSERT_STREQ(buf, "http://0.0.0.0:8080");
  memset(buf, 0, buf_sz);
  tok = find_json_token(arr, "peer[1]");
  memcpy(buf, tok->ptr, tok->len);
  ASSERT_STREQ(buf, "http://0.0.0.0:8081");
  memset(buf, 0, buf_sz);
  tok = find_json_token(arr, "peer[2]");
  memcpy(buf, tok->ptr, tok->len);
  ASSERT_STREQ(buf, "http://0.0.0.0:8082");
  memset(buf, 0, buf_sz);
  ASSERT_EQ(find_json_token(arr, "peer[3]"), (void *) NULL);
  free(arr);
}

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
  free(arr);
}

TEST(JsonTests, NestedJson) {
  static const char *json = "{foo:{foo:1},bar:2}";
  char buf[16] = {0};
  struct json_token *arr, *tok;
  arr = parse_json2(json, strlen(json));
  tok = find_json_token(arr, "foo");
  memcpy(buf, tok->ptr, tok->len);
  ASSERT_STREQ(buf, "{foo:1}");
  memset(buf, 0, 16);
  tok = find_json_token(arr, "foo.foo");
  memcpy(buf, tok->ptr, tok->len);
  ASSERT_STREQ(buf, "1");
  memset(buf, 0, 16);
  tok = find_json_token(arr, "bar");
  memcpy(buf, tok->ptr, tok->len);
  ASSERT_STREQ(buf, "2");
  memset(buf, 0, 16);
  free(arr);
}

TEST(JsonTests, ListJson) {
  static const char *json = "{\"foo\": [\"a\", \"b\", \"c\"]}";
  char buf[32] = {0};
  struct json_token *arr, *tok;
  arr = parse_json2(json, strlen(json));
  tok = find_json_token(arr, "foo");
  memcpy(buf, tok->ptr, tok->len);
  ASSERT_STREQ(buf, "[\"a\", \"b\", \"c\"]");
  memset(buf, 0, 16);
  tok = find_json_token(arr, "foo[0]");
  memcpy(buf, tok->ptr, tok->len);
  ASSERT_STREQ(buf, "a");
  memset(buf, 0, 16);
  tok = find_json_token(arr, "foo[1]");
  memcpy(buf, tok->ptr, tok->len);
  ASSERT_STREQ(buf, "b");
  memset(buf, 0, 16);
  tok = find_json_token(arr, "foo[2]");
  memcpy(buf, tok->ptr, tok->len);
  ASSERT_STREQ(buf, "c");
  memset(buf, 0, 16);
  tok = find_json_token(arr, "foo[3]");
  ASSERT_EQ(tok, (void *) NULL);
  memset(buf, 0, 16);
  free(arr);
}
