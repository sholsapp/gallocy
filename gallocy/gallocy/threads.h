#ifndef GALLOCY_THREADS_H_
#define GALLOCY_THREADS_H_

#include <pthread.h>
#include <stdint.h>
#include <sys/mman.h>


#define PAGE_SZ 4096


void* page_align_ptr(void *p);
void* allocate_thread_stack(void *location, size_t stack_size);

//
// pthread types
//

typedef
  void* (*thread_function_type) (void *);

typedef
  int (*pthread_create_function) (pthread_t *thread,
    const pthread_attr_t *attr,
    thread_function_type start_routine,
    void *arg);

typedef
  int (*pthread_join_function) (pthread_t thread, void **value_ptr);

typedef
  int (*pthread_mutex_init_function) (pthread_mutex_t *mutex,
    const pthread_mutexattr_t *attr);

typedef
  int (*pthread_mutex_destroy_function) (pthread_mutex_t *mutex);

typedef
  int (*pthread_mutex_lock_function) (pthread_mutex_t *mutex);

typedef
  int (*pthread_mutex_unlock_function) (pthread_mutex_t *mutex);

typedef
  int (*pthread_mutex_trylock_function) (pthread_mutex_t *mutex);

//
// pthread replacements for external applications
//

extern "C" int pthread_create(pthread_t *thread,
  const pthread_attr_t *attr,
  void *(*start_routine)(void *),
  void *arg) throw();

extern "C" int pthread_join(pthread_t thread, void **value_ptr);

//
// pthread references for internal library
//

extern "C" pthread_create_function __gallocy_pthread_create;
extern "C" pthread_join_function __gallocy_pthread_join;

#endif  // GALLOCY_THREADS_H_
