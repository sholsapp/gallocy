#include <curl/curl.h>
#include <dlfcn.h>

#include <iostream>

#include "gallocy/client.h"
#include "gallocy/config.h"
#include "gallocy/entrypoint.h"
#include "gallocy/logging.h"
#include "gallocy/models.h"
#include "gallocy/server.h"
#include "gallocy/threads.h"

int initialize_gallocy_framework(const char* config_path) {

  char *error;

  LOG_INFO("Initializing gallocy framework!");

  void *handle = dlopen("pthread.so", RTLD_GLOBAL);
  //
  // pthread_create
  //
  dlerror();
  __gallocy_pthread_create = reinterpret_cast<pthread_create_function>
    (reinterpret_cast<uint64_t *>(dlsym(handle, "pthread_create")));
  if ((error = dlerror()) != NULL)  {
    LOG_ERROR("Failed to set pthread_create: " << error);
  }
  LOG_INFO("Initialized __gallocy_pthread_create ["
      << &__gallocy_pthread_create
      << "] from pthread_create ["
      << reinterpret_cast<uint64_t *>(pthread_create)
      << "]");
  //
  // pthread_join
  //
  dlerror();
  __gallocy_pthread_join = reinterpret_cast<pthread_join_function>
    (reinterpret_cast<uint64_t *>(dlsym(handle, "pthread_join")));
  if ((error = dlerror()) != NULL) {
    LOG_ERROR("Failed to set pthread_join: " << error);
  }
  LOG_INFO("Initialized __gallocy_pthread_join ["
      << &__gallocy_pthread_join
      << "] from pthread_join ["
      << reinterpret_cast<uint64_t *>(pthread_join)
      << "]");

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

  LOG_INFO("Reading in config from " << config_path);

  GallocyConfig config = load_config(config_path);

  GallocyClient client(config);
  GallocyServer server(config);

  client.start();
  server.start();

  return 0;
}
