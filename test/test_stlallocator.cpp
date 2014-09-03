#include <cstdio>
#include <map>

volatile int anyThreadCreated = 0;

#include "heaplayers/myhashmap.h"
#include "heaplayers/spinlock.h"
#include "heaplayers/lockedheap.h"
#include "heaplayers/freelistheap.h"
#include "heaplayers/firstfitheap.h"
#include "heaplayers/zoneheap.h"
#include "heaplayers/source.h"
#include "heaplayers/stl.h"


class MainHeap :
  public HL::LockedHeap<HL::SpinLockType, HL::FreelistHeap<HL::ZoneHeap<SingletonHeap, 16384 - 16> > > {};


typedef
  std::map<int, int,
  std::less<int>,
  STLAllocator<std::pair<int, int>, MainHeap> >
    MyMap;


typedef
  std::vector<int,
  STLAllocator<int, MainHeap> >
    MyList;


int main(int argc, char* argv[]) {

  int i = 0;

  MyList mylist;
  for (int i = 1; i <= 10; i++)

    fprintf(stderr, "PUSHING %d\n", i);
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
