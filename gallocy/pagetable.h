#ifndef _PAGETABLE_H
#define _PAGETABLE_H


#include <map>
#include <string>
#include <pthread.h>
#include <sqlite3.h>


void init_sqlite_memory();


typedef struct condition_callback_param {
  pthread_mutex_t *mutex;
  pthread_cond_t *cv;
} condition_callback_param;


// TODO: read about cleaner ways to do this at
// http://stackoverflow.com/questions/1151582/pthread-function-from-a-class.
class Scheduler {
  public:
    Scheduler() {
      pthread_mutex_init(&page_table_initd_mutex, NULL);
      pthread_cond_init(&page_table_initd_cv, NULL);
      pthread_create(&master_thread, NULL, &Scheduler::construct_page_table, NULL);
    }

    static void *construct_page_table(void *ptr) {
      fprintf(stderr, "construct_page_table\n");
      return NULL;
    }

    static void *construct_network_stack(void *ptr) {
      fprintf(stderr, "construct_network_stack\n");
      return NULL;
    }

  private:
    pthread_t master_thread;
    pthread_mutex_t page_table_initd_mutex;
    pthread_cond_t page_table_initd_cv;
};


class PageTable {

  public:
    //PageTable() : database_path("pagetable.db") {
    PageTable() : database_path(":memory:") {
      init_sqlite_memory();
      open_database();
      create_tables();
    }

    void open_database();
    void create_tables();
    void insert_page_table_entry(void* ptr, int ptr_sz);

    static int noop_callback(void *not_used, int argc, char **argv, char **az_col_name);
    static int print_callback(void *not_used, int argc, char **argv, char **az_col_name);
    static int condition_callback(void *cond_param, int argc, char **argv, char **az_col_name);

  private:

    sqlite3 *db;
    const char *database_path;

};


extern PageTable pt;


#endif
