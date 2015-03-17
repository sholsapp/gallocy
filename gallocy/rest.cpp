#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <curl/curl.h>

#include "libgallocy.h"
#include "restclient.h"


void *curl_malloc(size_t sz) {
  return singletonHeap.malloc(sz);
}


void curl_free(void *ptr) {
  singletonHeap.free(ptr);
}


void* curl_realloc(void* ptr, size_t sz) {
  if (ptr == NULL) {
    return singletonHeap.malloc(sz);
  }
  size_t min_size = singletonHeap.getSize(ptr);
  void* buf = singletonHeap.malloc(sz);
  if (buf != NULL) {
    memcpy(buf, ptr, min_size);
    singletonHeap.free(ptr);
  }
  return buf;
}


char *curl_strdup(const char *s1) {
  char *s2 = (char *) singletonHeap.malloc(sizeof(char) * strlen(s1) + 1);
  memcpy(s2, s1, strlen(s1) + 1);
  return s2;
}


void *curl_calloc(size_t count, size_t size) {
  void *ptr = singletonHeap.malloc(count * size);
  memset(ptr, 0, count * size);
  return ptr;
}


int main(void) {

  curl_global_init_mem(
    CURL_GLOBAL_ALL,
    curl_malloc,
    curl_free,
    curl_realloc,
    curl_strdup,
    curl_calloc
  );

  RestClient::response r = RestClient::post("http://localhost:8080/",
      "text/json", "{\"foo\": \"bla\"}");
  fprintf(stdout, "CODE: %d\n", r.code);
  fprintf(stdout, "BODY: %s\n", r.body.c_str());

  return 0;

}
