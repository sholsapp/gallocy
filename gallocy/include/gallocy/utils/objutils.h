#ifndef GALLOCY_OBJUTILS_H_
#define GALLOCY_OBJUTILS_H_

#include <iostream>
#include <map>
#include <vector>

#include "gallocy/libgallocy.h"

namespace utils {

template <typename T>
void pretty_print_vector(gallocy::vector<T> v) {
  std::cout << "Vector(" << &v << ") {" << std::endl;
  for (auto it = std::begin(v); it != std::end(v); ++it) {
    std::cout << *it;
    if (std::distance(it, std::end(v)) > 1)
      std::cout << ", ";
  }
  std::cout << std::endl << "}" << std::endl;
}


template <typename K, typename V>
void pretty_print_map(gallocy::map<K, V> m) {
  std::cout << "Map(" << &m << ") {" << std::endl;
  for (auto it : m)
    std::cout << "    " << it->first << ": " << it->second << std::endl;
  std::cout << "}" << std::endl;
}

}  // namespace utils

#endif  // GALLOCY_OBJUTILS_H_
