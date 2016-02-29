#include <curl/curl.h>
#include <dlfcn.h>

#include <chrono>
#include <iostream>
#include <thread>

#include "gallocy/consensus/client.h"
#include "gallocy/consensus/machine.h"
#include "gallocy/consensus/server.h"
#include "gallocy/consensus/state.h"
#include "gallocy/entrypoint.h"
#include "gallocy/models.h"
#include "gallocy/threads.h"
#include "gallocy/utils/config.h"
#include "gallocy/utils/constants.h"
#include "gallocy/utils/logging.h"

GallocyClient *gallocy_client = nullptr;
GallocyConfig *gallocy_config = nullptr;
GallocyMachine *gallocy_machine = nullptr;
GallocyServer *gallocy_server = nullptr;
GallocyState *gallocy_state = nullptr;

int initialize_gallocy_framework(const char* config_path) {
  void *start;
  void *end;
  LOG_DEBUG("Initializing gallocy framework!");
  //
  // Fail early if a configuration file wasn't found.
  //
  if (!config_path) {
    LOG_ERROR("No configuration file provided!");
    abort();
  }
  //
  // Share where heaps have been placed.
  //
  start = get_heap_location(PURPOSE_INTERNAL_HEAP);
  end = reinterpret_cast<uint8_t *>(start) + ZONE_SZ;
  LOG_DEBUG("Set internal heap "
      << "[" << start << " - " << end << "]");
  start = get_heap_location(PURPOSE_SHARED_HEAP);
  end = reinterpret_cast<uint8_t *>(start) + ZONE_SZ;
  LOG_DEBUG("Set shared heap "
      << "[" << start << " - " << end << "]");
  start = get_heap_location(PURPOSE_APPLICATION_HEAP);
  end = reinterpret_cast<uint8_t *>(start) + ZONE_SZ;
  LOG_DEBUG("Set application heap "
      << "[" << start << " - " << end << "]");
  //
  // Initialize libcurl memory allocator.
  //
  curl_global_init(CURL_GLOBAL_NOTHING);
#if 0
  if (curl_global_init_mem(
      // CURL_GLOBAL_ALL,
      CURL_GLOBAL_NOTHING,
      malloc,
      free,
      realloc,
      strdup,
      calloc)) {
    LOG_ERROR("Failed to set curl global initiatilization settings.");
  }
#endif

  char *error;
  void *handle;
  //
  // Seed random number generator
  //
  std::srand(std::time(0));
#if __linux__
  const char* libpthread = "/lib/x86_64-linux-gnu/libpthread.so.0";
#else
  const char* libpthread = "/lib/x86_64-linux-gnu/libpthread.so.0";
  LOG_ERROR("We don't know how to find libpthread.so on this platform.");
#endif
  dlerror();
  handle = dlopen(libpthread, RTLD_LAZY);
  if (!handle) {
    LOG_ERROR("Failed to open " << libpthread << ": " << dlerror());
  }
  //
  // pthread_create
  //
  dlerror();
  __gallocy_pthread_create = reinterpret_cast<pthread_create_function>
    (reinterpret_cast<uint64_t *>(dlsym(handle, "pthread_create")));
  if ((error = dlerror()) != NULL)  {
    LOG_ERROR("Failed to set pthread_create: " << error);
  }
  LOG_DEBUG("Initialized __gallocy_pthread_create ["
      << &__gallocy_pthread_create
      << "] from pthread_create ["
      << reinterpret_cast<uint64_t *>(*__gallocy_pthread_create)
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
  LOG_DEBUG("Initialized __gallocy_pthread_join ["
      << &__gallocy_pthread_join
      << "] from pthread_join ["
      << reinterpret_cast<uint64_t *>(*__gallocy_pthread_join)
      << "]");
  //
  // Initialize SQLite memory allocator.
  //
  e.initialize();
  e.execute(PeerInfo::CREATE_STATEMENT);
  //
  // Load configuration file.
  //
  gallocy_config = load_config(config_path);
  //
  // Create the state object.
  //
  gallocy_state = new (internal_malloc(sizeof(GallocyState))) GallocyState(*gallocy_config);
  //
  // Create the client object.
  //
  gallocy_client = new (internal_malloc(sizeof(GallocyClient))) GallocyClient(*gallocy_config);
  //
  //
  // Start the client thread.
  //
  gallocy_machine = new (internal_malloc(sizeof(GallocyMachine))) GallocyMachine(*gallocy_config);
  gallocy_machine->start();
  //
  // Start the server thread.
  //
  gallocy_server = new (internal_malloc(sizeof(GallocyServer))) GallocyServer(*gallocy_config);
  gallocy_server->start();
  //
  // Yield to the application.
  //
  return 0;
}


int teardown_gallocy_framework() {
  gallocy_server->stop();
  gallocy_machine->stop();
  // TODO(sholsapp): Destroy the SQLite objects.
  // TODO(sholsapp): Destroy server, client, config objects and free memory.
  return 0;
}
