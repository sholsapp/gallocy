#include <cstdio>
#include <map>

#include "gtest/gtest.h"

volatile int anyThreadCreated = 0;

#include "libgallocy.h"
#include "heaplayers/stl.h"


TEST(STLTests, VectorTest) {
  int i = 0;
  gallocy::vector<int> mylist;
  for (int i = 0; i < 4096; i++)
    mylist.push_back(i);
  for (int i = 0; i < mylist.size(); i++)
    ASSERT_EQ(mylist[i], i) << "Failed on iteration [" << i << "]";
  mylist.clear();
  ASSERT_EQ(mylist.size(), 0);
}

TEST(STLTests, VectorCopyTest) {
  int i = 0;
  gallocy::vector<int> mylist;
  for (int i = 0; i < 4096; i++)
    mylist.push_back(i);
  for (int i = 0; i < mylist.size(); i++)
    ASSERT_EQ(mylist[i], i) << "Failed on iteration [" << i << "]";
  gallocy::vector<int> mylist2 = mylist;
  ASSERT_NE(&mylist2, &mylist);
  for (int i = 0; i < mylist.size(); i++)
    ASSERT_EQ(mylist2[i], i) << "Failed on iteration [" << i << "]";
  ASSERT_EQ(mylist.size(), 4096);
  mylist.clear();
  ASSERT_EQ(mylist.size(), 0);
  ASSERT_EQ(mylist2.size(), 4096);
  mylist2.clear();
  ASSERT_EQ(mylist2.size(), 0);
}


TEST(STLTests, MapTest) {
  gallocy::map<int, int> mymap;
  gallocy::map<int, int>::iterator it;
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
  gallocy::string s("abc123");
  ASSERT_EQ(s, "abc123");
  s.clear();
  ASSERT_EQ(s, "");
}


TEST(STLTests, StringManipulationTest) {
  gallocy::string a("aaa");
  gallocy::string b("bbb");
  ASSERT_EQ(a, "aaa");
  ASSERT_EQ(b, "bbb");
  ASSERT_EQ(a + b, "aaabbb");
  ASSERT_EQ(a + b + "ccc", "aaabbbccc");
  a.push_back('a');
  ASSERT_EQ(a, "aaaa");
}


TEST(STLTests, StringStreamTest) {
  gallocy::stringstream stream;
  stream << "aaa";
  stream << "bbb";
  ASSERT_EQ(stream.str(), "aaabbb");
}
