#include <iostream>

#include "./config.h"
#include "./constants.h"
#include "./httpd.h"
#include "./libgallocy.h"


int main(void) {
  uint16_t port = 8080;
  HTTPServer server(port);
  server.start();
  return 0;
}
