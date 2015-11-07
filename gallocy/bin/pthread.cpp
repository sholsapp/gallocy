#include <limits.h>
#include <pthread.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "gallocy/threads.h"


void* func(void* args) {
  std::cout << "Hello, World!" << std::endl;
  return nullptr;
}


int main(int argc, char* argv[]) {
  pthread_attr_t tattr;
  pthread_t tid;

  int ret = 0;
  void *stackbase = nullptr;

  int pages = 4;
  int size = 4096 * pages;

  // stackbase = (void *) malloc(size);
  stackbase = allocate_thread_stack(nullptr, pages);

  /* initialized with default attributes */
  ret = pthread_attr_init(&tattr);

  /* setting the stack */
  ret = pthread_attr_setstack(&tattr, stackbase, size);

  /* address and size specified */
  ret = pthread_create(&tid, &tattr, func, nullptr);

  ret = pthread_join(tid, nullptr);

  std::cout << ret << std::endl;

  return 0;
}
