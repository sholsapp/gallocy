#include <cstdio>
#include <map>

#include "gtest/gtest.h"

volatile int anyThreadCreated = 0;

#include "libgallocy.h"
#include "heaplayers/stl.h"


class STLTestHeap :
  public HL::LockedHeap<HL::SpinLockType, SingletonHeap> {};


typedef
  std::map<int, int,
  std::less<int>,
  STLAllocator<std::pair<int, int>, STLTestHeap> >
    MyMap;


typedef
  std::vector<int,
  STLAllocator<int, STLTestHeap> >
    MyList;


TEST(STLTests, VectorTest) {
  int i = 0;
  MyList mylist;
  for (int i = 0; i <= 10; i++)
    mylist.push_back(i);
  for (int i = 0; i < mylist.size(); i++)
    ASSERT_EQ(mylist[i], i);
  mylist.clear();
}


TEST(STLTests, MapTest) {
  MyMap mymap;
  MyMap::iterator it;
  mymap[1] = 1;
  mymap[2] = 4;
  mymap[3] = 9;
  mymap[4] = 16;
  it = mymap.find(3);
  mymap.erase(it);
  mymap[3] = 9;
  ASSERT_EQ(mymap[1], 1);
  ASSERT_EQ(mymap[2], 4);
  ASSERT_EQ(mymap[3], 9);
  ASSERT_EQ(mymap[4], 16);
  for (it = mymap.begin(); it != mymap.end(); it++)
    ASSERT_EQ((*it).second, mymap[(*it).first]);
}
