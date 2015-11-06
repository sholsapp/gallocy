#include <curl/curl.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "./config.h"
#include "./libgallocy.h"
#include "./restclient.h"


int main(void) {
  curl_global_init_mem(
    CURL_GLOBAL_ALL,
    internal_malloc,
    internal_free,
    internal_realloc,
    internal_strdup,
    internal_calloc);

  gallocy::string peer = "http://localhost:8080";

  RestClient::response r = RestClient::post(peer.c_str(),
      "text/json", "{\"foo\": \"bla\"}");
  std::cout << "[" << r.code << "] " << peer << std::endl;
  if (r.code == 200) {
    std::cout << "SUCCESS" << std::endl;
  } else {
    std::cout << "FAIL" << std::endl;
  }

#if 0
  gallocy::string host;
  int port;
  gallocy::string me;
  peer_list_t peers;

  read_config(host, port, me, peers);

  for (auto peer : peers) {
    RestClient::response r = RestClient::post(peer.c_str(),
        "text/json", "{\"foo\": \"bla\"}");
    std::cout << "[" << r.code << "] " << peer << std::endl;
    if (r.code == 200) {
      std::cout << "SUCCESS" << std::endl;
    } else {
      std::cout << "FAIL" << std::endl;
    }
  }
#endif

  return 0;
}
