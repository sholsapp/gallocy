#ifndef GALLOCY_ENTRYPOINT_H_
#define GALLOCY_ENTRYPOINT_H_

#include "gallocy/client.h"
#include "gallocy/config.h"
#include "gallocy/server.h"

/**
 * Initialize the gallocy framework.
 *
 * Initialization will::
 *
 *   - Configure internal memory allocator.
 *   - Configure internal database models.
 *   - Replace application memory allocator.
 *   - Replace application pthread interface.
 *   - Instantiate server.
 *   - Instantiate client.
 *
 * This should be called *before* the main function in the application.
 * After initialization, an application can begin executing application
 * logic as normal.
 */
int initialize_gallocy_framework(const char* config_path);

/**
 * Tear down the gallocy framework.
 *
 * Tear down will::
 *
 *   - Destroy server.
 *   - Destroy client.
 *
 * This should be called *after* the main function in the application exits.
 */
int teardown_gallocy_framework();

/**
 * The global handle to the server.
 */
extern GallocyServer *gallocy_server;

/**
 * The global handle to the client.
 */
extern GallocyClient *gallocy_client;

/**
 * The global handle to the configuration.
 */
extern GallocyConfig *gallocy_config;

#endif  // GALLOCY_ENTRYPOINT_H_
