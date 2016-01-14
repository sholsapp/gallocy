#ifndef GALLOCY_ENTRYPOINT_H_
#define GALLOCY_ENTRYPOINT_H_

/**
 * Initialize the gallocy framework.
 *
 * Initialization will::
 *
 *   - Configure internal memory allocator.
 *   - Configure internal database models.
 *   - Replace application memory allocator.
 *   - Replace application pthread interface.
 *
 * This should be called *before* the main function in the application.
 * After initialization, an application can begin executing application
 * logic as normal.
 */
int initialize_gallocy_framework(const char* config_path);

#endif  // GALLOCY_ENTRYPOINT_H_
