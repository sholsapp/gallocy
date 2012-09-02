#include <cstring>
#include <cstdlib>
#include <cstdio>

//#ifdef __APPLE__
//#include <malloc/malloc.h>
//#elif
#include <malloc.h>
//#endif

extern "C" {

  // Declare symbols, but define them somewhere else
  void* custom_malloc(size_t);
  void custom_free(void *);

  // New hooks for allocation functions.
  static void* my_malloc_hook (size_t, const void *);
  static void my_free_hook (void *, const void *);

  // Store the old hooks just in case.
  static void * (*old_malloc_hook) (size_t, const void *);
  static void (*old_free_hook) (void *, const void *);

  static void * my_malloc_hook (size_t size, const void *) {
    void * result = custom_malloc(size);
    return result;
  }

  static void my_free_hook (void * ptr, const void *) {
    custom_free(ptr);
  }

  static void my_init_hook (void) {

    // Store the old hooks.
    old_malloc_hook = __malloc_hook;
    old_free_hook = __free_hook;

    // Point the hooks to the replacement functions.
    __malloc_hook = my_malloc_hook;
    __free_hook = my_free_hook;

  }

  void (*__malloc_initialize_hook) (void) = my_init_hook;

}
