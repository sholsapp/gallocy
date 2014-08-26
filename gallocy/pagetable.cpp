#include <string>
#include <sstream>
#include <stdio.h>
#include <unistd.h>

#include "pagetable.h"


// These should already be included, but include them again to be explicit.
#include "heaplayers/myhashmap.h"
#include "heaplayers/spinlock.h"
#include "heaplayers/lockedheap.h"
#include "heaplayers/freelistheap.h"
#include "heaplayers/firstfitheap.h"
#include "heaplayers/zoneheap.h"
#include "heaplayers/source.h"
#include "heaplayers/stl.h"


class xSizeHeap :
  public HL::LockedHeap<HL::SpinLockType, HL::FreelistHeap<HL::ZoneHeap<SingletonHeap, 16384 - 16> > > {};


typedef
  std::map<void*, int,
  std::less<void*>,
  STLAllocator<std::pair<void*, int>, xSizeHeap> >
    xSizeMapType;


xSizeMapType xSizeMap;


class SqliteHeap: public SourceHeap {};


SqliteHeap sqliteHeap;


void* xMalloc(int sz) {
  if (sz == 0) {
    return NULL;
  }
  void* ptr = sqliteHeap.malloc(sz);
  xSizeMap.insert(std::pair<void*, int>(ptr, sz));
  //fprintf(stderr, "%p = xMalloc(%d)\n", ptr, sz);
  return ptr;
}


void xFree(void* ptr) {
  //fprintf(stderr, "xFree(%p)\n", ptr);
  xSizeMap.erase(ptr);
  sqliteHeap.free(ptr);
  return;
}


void* xRealloc(void* ptr, int sz) {
  if (ptr == NULL) {
    return sqliteHeap.malloc(sz);
  }
  size_t min_size = sqliteHeap.getSize(ptr);
  void* buf = sqliteHeap.malloc(sz);
  if (buf != NULL) {
    memcpy(buf, ptr, min_size);
    sqliteHeap.free(ptr);
  }
  xSizeMap.insert(std::pair<void*, int>(buf, sz));
  //fprintf(stderr, "%p = xRealloc(%p, %d)\n", buf, ptr, sz);
  return buf;
}


int xSize(void* ptr) {
  int sz = xSizeMap[ptr];
  //fprintf(stderr, "%d = xSize(%p)\n", sz, ptr);
  return sz;
}


int xRoundup(int sz) {
  //fprintf(stderr, "xRoundup(%d)\n", sz);
  return sz;
}


int xInit(void* ptr) {
  fprintf(stderr, "xInit\n");
  return 0;
}


void xShutdown(void* ptr) {
  fprintf(stderr, "xShutdown\n");
  return;
}


sqlite3_mem_methods my_mem = {
  &xMalloc,
  &xFree,
  &xRealloc,
  &xSize,
  &xRoundup,
  &xInit,
  &xShutdown,
  0
};


void init_sqlite_memory() {
  if (sqlite3_config(SQLITE_CONFIG_MALLOC, &my_mem) != SQLITE_OK) {
    fprintf(stderr, "Failed to set custom sqlite memory allocator!\n");
    return;
  }
  fprintf(stderr, "Successfully set custom sqlite memory allocator!\n");
  return;
}


int PageTable::noop_callback(void *not_used, int argc, char **argv, char **az_col_name) {
   return 0;
}


int PageTable::print_callback(void *not_used, int argc, char **argv, char **az_col_name) {
  int i;
  for(i=0; i<argc; i++) {
    printf("%s = %s\n", az_col_name[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}


int PageTable::condition_callback(void *cond_param, int argc, char **argv, char **az_col_name) {
  condition_callback_param *param = (condition_callback_param *) cond_param;
  pthread_mutex_lock(param->mutex);
  pthread_cond_signal(param->cv);
  pthread_mutex_unlock(param->mutex);
  return 0;
}


void PageTable::open_database() {
  int  rc;
  rc = sqlite3_open(database_path, &db);
  if(rc) {
     fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
     exit(0);
  } else {
     fprintf(stdout, "Opened database successfully\n");
  }
}


void PageTable::create_tables() {
  char *zErrMsg = 0;
  int  rc;
  char *sql;
  sql = "CREATE TABLE pagetable (" \
        "id INT PRIMARY KEY     NOT NULL,    " \
        "address        INT     NOT NULL,    " \
        "size           INT     NOT NULL);   ";
  rc = sqlite3_exec(db, sql, print_callback, 0, &zErrMsg);
  if(rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    fprintf(stdout, "Table created successfully\n");
  }
  return;
}


void PageTable::insert_page_table_entry(void* ptr, int ptr_sz) {
  static int unique_id = 0;
  char *zErrMsg = 0;
  int  rc;
  char sql[256];

  // NOTE: this doesn't work because std::stringstream uses memory in a way
  // that causes infinite loop. I.e., it looks like application memory.
  //std::stringstream sql;
  //sql << "INSERT INTO APPLICATION_PAGE_TABLE (ID,ADDRESS) " \
  //      "VALUES (" << unique_id++ << "," << (intptr_t) ptr << "); ";

  snprintf(
    sql,
    256,
    "INSERT INTO pagetable (id,address,size)" \
    "VALUES (%d, %ld, %d);",
    unique_id,
    (intptr_t) ptr,
    ptr_sz);

  unique_id++;

  /* Execute SQL statement */
  rc = sqlite3_exec(db, sql, print_callback, 0, &zErrMsg);
  if(rc != SQLITE_OK) {
     fprintf(stderr, "SQL error: %s\n", zErrMsg);
     sqlite3_free(zErrMsg);
  }
  return;
}


PageTable pt;
//Scheduler s;
