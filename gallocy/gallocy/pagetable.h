#ifndef GALLOCY_PAGETABLE_H_
#define GALLOCY_PAGETABLE_H_

#if 0
#include <pthread.h>
#include <sqlite3.h>

#include <cstdio>
#include <cstdlib>


void init_sqlite_memory();


typedef struct condition_callback_param {
  pthread_mutex_t *mutex;
  pthread_cond_t *cv;
} condition_callback_param;


class PageTable {
 public:
    PageTable() : database_path(":memory:") {
      init_sqlite_memory();
      open_database();
      create_tables();
    }

    void open_database();
    void create_tables();
    void insert_page_table_entry(void* ptr, int ptr_sz);
    int get_page_table_entry_count();

    static int noop_callback(void *not_used, int argc, char **argv, char **az_col_name);
    static int print_callback(void *not_used, int argc, char **argv, char **az_col_name);
    static int condition_callback(void *cond_param, int argc, char **argv, char **az_col_name);

 private:
    sqlite3 *db;
    const char *database_path;
};

extern PageTable pt;
#endif

#endif  // GALLOCY_PAGETABLE_H_
