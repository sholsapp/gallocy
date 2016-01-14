#ifndef GALLOCY_ALLOCATORS_INTERNAL_H_
#define GALLOCY_ALLOCATORS_INTERNAL_H_

#include <functional>
#include <map>
#include <sstream>
#include <streambuf>
#include <string>
#include <utility>
#include <vector>

#include "external/json.hpp"
#include "heaplayers/internal.h"
#include "heaplayers/stl.h"



extern HL::InternalMemoryHeap local_internal_memory;


namespace gallocy {

// Use a C++11 alias declaration here, as opposed to inheritance, so that the
// the new declaration need not reimplement constructors.

template <typename T>
using ContainerAllocator = STLAllocator<T, HL::InternalMemoryHeap>;


using gallocy_string = std::basic_string<char,
      std::char_traits<char>,
      ContainerAllocator<char> >;
class string: public gallocy_string {
 public:
  // Inherit all of the constructors from the base class.
  using gallocy_string::gallocy_string;
  string() {}
  string(const gallocy_string &s) {  // NOLINT(runtime/explicit)
    *this = string(s.c_str());
  }
  string(const std::string &s) {  // NOLINT(runtime/explicit)
    *this = string(s.c_str());
  }
  operator const char *() { return this->c_str(); }
  operator const gallocy_string () { return this->c_str(); }
  operator const std::string () { return this->c_str(); }
};


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

}  // namespace gallocy


extern "C" {
  void *internal_malloc(size_t sz);
  void internal_free(void *ptr);
  void *internal_realloc(void *ptr, size_t sz);
  char *internal_strdup(const char *ptr);
  void *internal_calloc(size_t num, size_t sz);

}

#endif  // GALLOCY_ALLOCATORS_INTERNAL_H_
