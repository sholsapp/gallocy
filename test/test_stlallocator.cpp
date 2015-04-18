#include <cstdio>
#include <map>

#include "gtest/gtest.h"

volatile int anyThreadCreated = 0;

#include "libgallocy.h"
#include "heaplayers/stl.h"


class STLTestHeap :
  public HL::SingletonHeap {};


typedef
  std::map<int, int,
  std::less<int>,
  STLAllocator<std::pair<int, int>, STLTestHeap> >
    MyMap;


typedef
  std::vector<int,
  STLAllocator<int, STLTestHeap> >
    MyList;


typedef
  std::basic_string<char,
  std::char_traits<char>,
  STLAllocator<
    std::basic_string<char>,
    STLTestHeap> >
      MyString;


TEST(STLTests, VectorTest) {
  int i = 0;
  MyList mylist;
  for (int i = 0; i < 4096; i++)
    mylist.push_back(i);
  for (int i = 0; i < mylist.size(); i++)
    ASSERT_EQ(mylist[i], i) << "Failed on iteration [" << i << "]";
  mylist.clear();
  ASSERT_EQ(mylist.size(), 0);
}


TEST(STLTests, MapTest) {
  MyMap mymap;
  MyMap::iterator it;
  for (int i = 0; i < 4096; i++)
    mymap[i] = i * i;
  ASSERT_EQ(mymap[1], 1);
  ASSERT_EQ(mymap[2], 4);
  ASSERT_EQ(mymap[3], 9);
  ASSERT_EQ(mymap[4], 16);
  for (it = mymap.begin(); it != mymap.end(); it++)
    ASSERT_EQ((*it).second, mymap[(*it).first]);
  mymap.clear();
  ASSERT_EQ(mymap.size(), 0);
}


TEST(STLTests, StringTest) {
  MyString s("abc123");
  ASSERT_EQ(s, "abc123");
  s.clear();
  ASSERT_EQ(s, "");
}


TEST(STLTests, StringManipulationTest) {
  MyString a("aaa");
  MyString b("bbb");
  ASSERT_EQ(a, "aaa");
  ASSERT_EQ(b, "bbb");
  ASSERT_EQ(a + b, "aaabbb");
  ASSERT_EQ(a + b + "ccc", "aaabbbccc");
  a.push_back('a');
  ASSERT_EQ(a, "aaaa");
}
