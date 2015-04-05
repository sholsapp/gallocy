#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <curl/curl.h>

#include "frozen.h"
#include "libgallocy.h"
#include "restclient.h"


int main(void) {

  curl_global_init_mem(
    CURL_GLOBAL_ALL,
    internal_malloc,
    internal_free,
    internal_realloc,
    internal_strdup,
    internal_calloc
  );
  allocator._realloc = internal_realloc;
  allocator._free = internal_free;

  RestClient::response r = RestClient::post("http://localhost:8080/",
      "text/json", "{\"foo\": \"bla\"}");

  struct json_token *arr, *tok;
  arr = parse_json2(r.body.c_str(), strlen(r.body.c_str()));
  tok = find_json_token(arr, "foo");

  char buf[256] = {0};
  if (r.code == 200 && tok) {
    memcpy(buf, tok->ptr, tok->len);
    fprintf(stderr, "JSON: %s -> %s\n", "foo", buf);
  }

  return 0;

}
