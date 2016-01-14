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

// TODO(sholsapp): This function needs to utilize the private heaps for most of
// these things, otherwise they'll go out of scope once the initialization
// function is finished.
int initialize_gallocy_framework(const char* config_path) {

  char *error;

  LOG_INFO("Initializing gallocy framework!");

  std::srand(std::time(0));

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
  e.execute(PeerInfo::CREATE_STATEMENT);

  GallocyConfig config = load_config(config_path);
  GallocyClient client(config);
  GallocyServer server(config);

  client.start();
  server.start();

  // The following is sample application logic.
  while (true) {
    int size = 8092 - std::rand() % 8092;
    char *memory = (char *) malloc(sizeof(char) * size);
    LOG_APP("allocated " << size << " byte(s) in " << reinterpret_cast<void *>(memory));
    memset(memory, '!', size);
    free(memory);
    int duration = 30 - std::rand() % 30;
    LOG_APP("sleeping for " << duration);
    sleep(duration);
  }

  return 0;
}
