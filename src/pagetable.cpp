#include <stdio.h>
#include <sqlite3.h>

#include "pagetable.h"


//typedef struct sqlite3_mem_methods sqlite3_mem_methods;
//struct sqlite3_mem_methods {
//  void *(*xMalloc)(int);         /* Memory allocation function */
//  void (*xFree)(void*);          /* Free a prior allocation */
//  void *(*xRealloc)(void*,int);  /* Resize an allocation */
//  int (*xSize)(void*);           /* Return the size of an allocation */
//  int (*xRoundup)(int);          /* Round up request size to allocation size */
//  int (*xInit)(void*);           /* Initialize the memory allocator */
//  void (*xShutdown)(void*);      /* Deinitialize the memory allocator */
//  void *pAppData;                /* Argument to xInit() and xShutdown() */
//};

typedef struct sqlite3_mem_methods sqlite3_mem_methods;
sqlite3_mem_methods my_mem = {
  0,
  0,
  0,
  0,
  0,
  0,
  0
};


void init_sqlite_memory() {
  fprintf(stderr, "HELLO\n");
  if (sqlite3_config(SQLITE_CONFIG_MALLOC, &my_mem) != SQLITE_OK) {
    fprintf(stderr, "Failed to set custom sqlite memory allocator!\n");
  }
}

