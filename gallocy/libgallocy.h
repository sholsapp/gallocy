#ifndef _LIBGALLOCY_H
#define _LIBGALLOCY_H

#include <vector>
#include <map>
#include <string>

#include "pagetable.h"
#include "firstfitheap.h"

#include "heaplayers/heaptypes.h"


extern volatile int anyThreadCreated;


extern MainHeapType heap;


extern SingletonHeapType singletonHeap;


namespace gallocy {


typedef std::basic_string<char,
  std::char_traits<char>,
  STLAllocator<char, SingletonHeapType> > string;


template <class T>
class vector : public std::vector
               <T, STLAllocator<T, SingletonHeapType> > {};


template <class K, class V>
class map : public std::map
            <K, V, std::less<K>,
            STLAllocator<std::pair<K, V>, SingletonHeapType> > {};


}


extern "C" {

  void *internal_malloc(size_t);
  void internal_free(void *);
  void *internal_realloc(void *, size_t);
  char *internal_strdup(const char *);
  void *internal_calloc(size_t, size_t);

}


extern "C" {

  void __reset_memory_allocator();
  void* custom_malloc(size_t);
  void custom_free(void*);
  void* custom_realloc(void*, size_t);
  // This is an OSX thing, but is useful for testing.
  size_t custom_malloc_usable_size(void*);
#ifdef __APPLE__
  void custom_malloc_lock();
  void custom_malloc_unlock();
#endif

}

#endif
