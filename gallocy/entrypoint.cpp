#include <curl/curl.h>
#include <dlfcn.h>

#include <iostream>

#include "gallocy/consensus/client.h"
#include "gallocy/consensus/server.h"
#include "gallocy/consensus/state.h"
#include "gallocy/entrypoint.h"
#include "gallocy/models.h"
#include "gallocy/threads.h"
#include "gallocy/utils/config.h"
#include "gallocy/utils/logging.h"

GallocyClient *gallocy_client = nullptr;
GallocyConfig *gallocy_config = nullptr;
GallocyServer *gallocy_server = nullptr;
GallocyState *gallocy_state = nullptr;

int initialize_gallocy_framework(const char* config_path) {
  LOG_DEBUG("Initializing gallocy framework!");
  //
  // Initialize libcurl memory allocator.
  //
  if (curl_global_init_mem(
      // TODO(sholsapp): libcurl causes an invalid free to happen in libcrypto
      // when this is enabled *and* the wrapper is linked with target
      // application (note: wrapper is linked means all memory functions have
      // been interposed in the target application, including libc/libc++ which
      // gallocy may be using internally). This is likely a problem with either
      // i) double free or ii) libc is handing a pointer from custom_malloc to
      // internal_free.
      //CURL_GLOBAL_ALL,
      CURL_GLOBAL_NOTHING,
// TODO(sholsapp): We are seeing double free corruption even when compiling
// libcurl ourselves with minimal library support. Thing about re-enabling this
// when we can static link standard library usages.
#if 0
      internal_malloc,
      internal_free,
      internal_realloc,
      internal_strdup,
      internal_calloc)) {
#endif
      malloc,
      free,
      realloc,
      strdup,
      calloc)) {
    LOG_ERROR("Failed to set curl global initiatilization settings.");
  }

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
  gallocy_state = new (internal_malloc(sizeof(GallocyState))) GallocyState();
  //
  //
  // Start the client thread.
  //
  gallocy_client = new (internal_malloc(sizeof(GallocyClient))) GallocyClient(*gallocy_config);
  gallocy_client->start();
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
  gallocy_client->stop();
  // TODO(sholsapp): Destroy the SQLite objects.
  // TODO(sholsapp): Destroy server, client, config objects and free memory.
  return 0;
}
