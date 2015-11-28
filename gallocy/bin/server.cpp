#include <iostream>

#include "gallocy/client.h"
#include "gallocy/config.h"
#include "gallocy/constants.h"
#include "gallocy/libgallocy.h"
#include "gallocy/server.h"


int main(int argc, char *argv[]) {
  GallocyConfig config = load_config(argv[1]);

  // TODO(sholsapp): Unify the naming here for consistency.
  GallocyClient client(config);
  GallocyServer server(config);

  client.start();
  server.start();
  return 0;
}
