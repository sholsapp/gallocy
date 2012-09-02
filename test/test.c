#include <cstdio>
#include <map>

#include "heaps/stl.h"
#include "heaps/source.h"

typedef
  std::map<int, int>
    TheirMap;

typedef
  std::map<int, int,
  std::less<int>,
  STLAllocator<std::pair<int, int>, SimpleHeap> >
    MyMap;

int main(int argc, char* argv[]) {

  TheirMap test;
  test[1] = 1;
  test[2] = 4;
  fprintf(stderr, "%d -> %d\n", 1, test[1]);
  fprintf(stderr, "%d -> %d\n", 2, test[2]);

  MyMap test2;
  test2[1] = 1;
  test2[2] = 4;
  fprintf(stderr, "%d -> %d\n", 1, test2[1]);
  fprintf(stderr, "%d -> %d\n", 2, test2[2]);

  return 0;
}
