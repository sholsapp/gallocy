#ifndef GALLOCY_HEAPLAYERS_FREESLLIST_H_
#define GALLOCY_HEAPLAYERS_FREESLLIST_H_

#include <assert.h>


class FreeSLList {
 public:
  class Entry {
   public:
      Entry(void)
      : next(NULL) {}
    Entry *next;
#if 1
   private:
    Entry(const Entry&);
    Entry &operator=(const Entry&);
#endif
  };

  inline void clear() {
    head.next = NULL;
  }

  inline Entry *get() {
    const Entry *e = head.next;
    if (e == NULL) {
      return NULL;
    }
    head.next = e->next;
    return const_cast<Entry *>(e);
  }

  inline Entry *remove() {
    const Entry * e = head.next;
    if (e == NULL) {
      return NULL;
    }
    head.next = e->next;
    return const_cast<Entry *>(e);
  }

  inline void insert(void *e) {
    Entry *entry = reinterpret_cast<Entry *>(e);
    entry->next = head.next;
    head.next = entry;
  }

 private:
  Entry head;
};

#endif  // GALLOCY_HEAPLAYERS_FREESLLIST_H_
