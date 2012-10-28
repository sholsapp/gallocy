#include <cstdio>
#include <map>

#include "heaps/stl.h"
#include "heaps/source.h"

typedef
  std::map<int, int,
  std::less<int>,
  STLAllocator<std::pair<int, int>, FreelistHeap<SimpleHeap> > >
    MyMap;

int main(int argc, char* argv[]) {
  MyMap mymap;
  MyMap::iterator it;
  mymap[1] = 1;
  mymap[2] = 4;
  mymap[3] = 9;
  mymap[4] = 16;
  it = mymap.find(3);
  mymap.erase(it);
  mymap[3] = 9;
  for (it = mymap.begin(); it != mymap.end(); it++) {
    fprintf(stderr, "%d->%d\n", (*it).first, (*it).second);
  }
  return 0;
}
