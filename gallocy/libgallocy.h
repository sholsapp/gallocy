#ifndef _LIBGALLOCY_H
#define _LIBGALLOCY_H

#include <map>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>

#include "pagetable.h"
#include "firstfitheap.h"

#include "heaplayers/heaptypes.h"


extern volatile int anyThreadCreated;


extern MainHeapType heap;


extern SingletonHeapType singletonHeap;


namespace gallocy {

// XXX: I can't figure out why we actually need this class. All if does is
// extends HL::SingletonHeap. Without it, the STLAllocator uses below fail.
// With it, the STLAllocators work. Here be dragons.
class __STLAllocator: public HL::SingletonHeap {};


typedef std::basic_string<char,
  std::char_traits<char>,
  STLAllocator<char, __STLAllocator> > string;


//typedef std::basic_stringbuf<char,
//  std::char_traits<char>,
//  STLAllocator<char, __STLAllocator> > stringbuf;


class stringstream: public std::basic_stringstream<char,
                    std::char_traits<char>,
                    STLAllocator<char, __STLAllocator> > {};


template <class T>
class vector : public std::vector
               <T, STLAllocator<T, __STLAllocator> > {};


template <class K, class V>
class map : public std::map
            <K, V, std::less<K>,
            STLAllocator<std::pair<K, V>, __STLAllocator> > {};

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
