#include <iostream>

#include "gallocy/config.h"
#include "gallocy/constants.h"
#include "gallocy/httpd.h"
#include "gallocy/libgallocy.h"


int main(void) {
  uint16_t port = 8080;
  HTTPServer server(port);
  server.start();
  return 0;
}
