#include <cstdio>
#include <cstdlib>
#include <limits.h>
#include <pthread.h>

#include "threads.h"


void* func(void* args) {
  fprintf(stderr, "Hello, World!\n");
  return NULL;
}


int main(int argc, char* argv[]) {

  pthread_attr_t tattr;
  pthread_t tid;

  int ret = 0;
  void *stackbase = NULL;

  int pages = 4;
  int size = 4096 * pages;

  //stackbase = (void *) malloc(size);
  stackbase = allocate_thread_stack(NULL, pages);

  /* initialized with default attributes */
  ret = pthread_attr_init(&tattr);

  /* setting the size of the stack also */
  ret = pthread_attr_setstacksize(&tattr, size);

  /* setting the base address in the attribute */
  ret = pthread_attr_setstackaddr(&tattr, stackbase);

  /* address and size specified */
  ret = pthread_create(&tid, &tattr, func, NULL);

  ret = pthread_join(tid, NULL);

  return 0;
}
