#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <curl/curl.h>

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

  RestClient::response r = RestClient::post("http://localhost:8080/",
      "text/json", "{\"foo\": \"bla\"}");
  fprintf(stdout, "CODE: %d\n", r.code);
  fprintf(stdout, "BODY: %s\n", r.body.c_str());

  return 0;

}
