#include <curl/curl.h>

#include <iostream>

#include "gallocy/client.h"
#include "gallocy/config.h"
#include "gallocy/constants.h"
#include "gallocy/libgallocy.h"
#include "gallocy/models.h"
#include "gallocy/server.h"


int main(int argc, char *argv[]) {
  curl_global_init_mem(
    CURL_GLOBAL_ALL,
    internal_malloc,
    internal_free,
    internal_realloc,
    internal_strdup,
    internal_calloc);

  e.initialize();
  // TODO(sholsapp): Move this into a "create_all" function so that we can
  // initialize the entire model without having to add something here every
  // time we add a new model.
  e.execute(PeerInfo::CREATE_STATEMENT);

  GallocyConfig config = load_config(argv[1]);

  GallocyClient client(config);
  GallocyServer server(config);

  client.start();
  server.start();
  return 0;
}
