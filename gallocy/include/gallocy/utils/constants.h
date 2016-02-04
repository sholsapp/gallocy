#ifndef GALLOCY_UTILS_CONSTANTS_H_
#define GALLOCY_UTILS_CONSTANTS_H_


#include <unistd.h>
#include <stdint.h>

#define PAGE_SZ 4096

// 32 MB of memory
#define ZONE_SZ   1024 * 1024 * 32

#define PURPOSE_DEVELOPMENT_HEAP  100
#define PURPOSE_INTERNAL_HEAP     101
#define PURPOSE_SHARED_HEAP       102
#define PURPOSE_APPLICATION_HEAP  103

/**
 * Return the address of the ``main`` function.
 */
extern uint64_t *&global_main();
/**
 * Return the address that demarks the end of the .TEXT region.
 */
extern uint64_t *&global_end();
/**
 * Return the starting address for use by internal memory allocators.
 */
extern uint64_t *&global_base();
/**
 * Get a heap's desired location.
 *
 * :returns: The desired address at which to start allocating memory.
 */
void *get_heap_location(uint64_t purpose);

#endif  // GALLOCY_UTILS_CONSTANTS_H_
