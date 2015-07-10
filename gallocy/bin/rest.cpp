#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <curl/curl.h>

#include "frozen.h"
#include "libgallocy.h"
#include "restclient.h"
#include "config.h"


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

  gallocy::string me;
  peer_list_t peers;
  read_config(me, peers);

  for (auto peer : peers) {
    RestClient::response r = RestClient::post(peer.c_str(),
        "text/json", "{\"foo\": \"bla\"}");
    std::cout << "[" << r.code << "] " << peer << std::endl;
    if (r.code == 200) {
    } else {
    }
  }

  RestClient::response r = RestClient::post("http://localhost:8080/admin",
      "text/json", "{\"foo\": \"bla\"}");
  if (r.code == 200) {
    struct json_token *arr, *tok;
    arr = parse_json2(r.body.c_str(), strlen(r.body.c_str()));
    tok = find_json_token(arr, "foo");
    char buf[256] = {0};
    if (r.code == 200 && tok) {
      memcpy(buf, tok->ptr, tok->len);
      fprintf(stderr, "JSON: %s -> %s\n", "foo", buf);
    }
    free(arr);
  } else {
    std::cout << "Bad response: " << r.code << std::endl;
  }

  return 0;

}
