#include <cstdio>
#include <map>

#include "heaps/stl.h"
#include "heaps/source.h"

typedef
  std::map<int, int,
  std::less<int>,
  STLAllocator<std::pair<int, int>, FreelistHeap<SimpleHeap> > >
    MyMap;


typedef
  std::vector<int,
  STLAllocator<int, FreelistHeap<SimpleHeap> > >
    MyList;


int main(int argc, char* argv[]) {

  MyList mylist;
  for (int i = 1; i <= 10; i++)
    mylist.push_back(i);
  for (int i = 0; i < mylist.size(); i++)
    fprintf(stderr, "%d -> %d\n", i, mylist[i]);
  mylist.clear();

  MyMap mymap;
  MyMap::iterator it;
  mymap[1] = 1;
  mymap[2] = 4;
  mymap[3] = 9;
  mymap[4] = 16;
  it = mymap.find(3);
  mymap.erase(it);
  mymap[3] = 9;
  for (it = mymap.begin(); it != mymap.end(); it++)
    fprintf(stderr, "%d->%d\n", (*it).first, (*it).second);

  return 0;
}
