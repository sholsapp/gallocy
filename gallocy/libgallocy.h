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

#include "external/json.hpp"


extern volatile int anyThreadCreated;


extern MainHeapType heap;


extern SingletonHeapType singletonHeap;


namespace gallocy {

// XXX: I can't figure out why we actually need this class. All if does is
// extends HL::SingletonHeap. Without it, the STLAllocator uses below fail.
// With it, the STLAllocators work. Here be dragons.
class __STLAllocator: public HL::SingletonHeap {};


/**
 * Use a C++11 alias declaration here, as opposed to inheritance, so that the
 * the new declaration need not reimplement constructors.
 */
template <typename T>
using ContainerAllocator = STLAllocator<T, __STLAllocator>;


using string = std::basic_string<char,
      std::char_traits<char>,
      ContainerAllocator<char> >;

using stringstream = std::basic_stringstream<char,
      std::char_traits<char>,
      ContainerAllocator<char> >;

template <typename T>
using vector = std::vector<T, ContainerAllocator<T> >;

template <typename K, typename V>
using map = std::map<K, V, std::less<K>, ContainerAllocator<std::pair<K, V> > >;

using json = nlohmann::basic_json<
  std::map,
  std::vector,
  std::string,
  bool,
  int64_t,
  double,
  ContainerAllocator>;

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
