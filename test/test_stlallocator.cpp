#include <cstdio>
#include <map>

#include "gtest/gtest.h"

volatile int anyThreadCreated = 0;

#include "libgallocy.h"
#include "heaplayers/stl.h"


TEST(STLTests, VectorTest) {
  gallocy::vector<uint64_t> mylist;
  for (uint64_t i = 0; i < 4096; i++)
    mylist.push_back(i);
  for (uint64_t i = 0; i < mylist.size(); i++)
    ASSERT_EQ(mylist[i], i) << "Failed on iteration [" << i << "]";
  mylist.clear();
  ASSERT_EQ(mylist.size(), static_cast<size_t>(0));
}

TEST(STLTests, VectorCopyTest) {
  gallocy::vector<uint64_t> mylist;
  for (uint64_t i = 0; i < 4096; i++)
    mylist.push_back(i);
  for (uint64_t i = 0; i < mylist.size(); i++)
    ASSERT_EQ(mylist[i], i) << "Failed on iteration [" << i << "]";
  gallocy::vector<uint64_t> mylist2 = mylist;
  ASSERT_NE(&mylist2, &mylist);
  for (uint64_t i = 0; i < mylist.size(); i++)
    ASSERT_EQ(mylist2[i], i) << "Failed on iteration [" << i << "]";
  ASSERT_EQ(mylist.size(), static_cast<size_t>(4096));
  mylist.clear();
  ASSERT_EQ(mylist.size(), static_cast<size_t>(0));
  ASSERT_EQ(mylist2.size(), static_cast<size_t>(4096));
  mylist2.clear();
  ASSERT_EQ(mylist2.size(), static_cast<size_t>(0));
}


TEST(STLTests, MapTest) {
  gallocy::map<uint64_t, uint64_t> mymap;
  gallocy::map<uint64_t, uint64_t>::iterator it;
  for (uint64_t i = 0; i < 4096; i++)
    mymap[i] = i * i;
  ASSERT_EQ(mymap[1], static_cast<size_t>(1));
  ASSERT_EQ(mymap[2], static_cast<size_t>(4));
  ASSERT_EQ(mymap[3], static_cast<size_t>(9));
  ASSERT_EQ(mymap[4], static_cast<size_t>(16));
  for (it = mymap.begin(); it != mymap.end(); it++)
    ASSERT_EQ((*it).second, mymap[(*it).first]);
  mymap.clear();
  ASSERT_EQ(mymap.size(), static_cast<size_t>(0));
}


TEST(STLTests, StringTest) {
  gallocy::string s("abc123");
  ASSERT_EQ(s, "abc123");
  s.clear();
  ASSERT_EQ(s, "");
}


TEST(STLTests, StringConstructors) {
  gallocy::string s1("abc");
  ASSERT_EQ(s1, "abc");
  gallocy::string s2 = "def";
  ASSERT_EQ(s2, "def");
  ASSERT_NE(&s1, &s2);
  s1 = s2;
  ASSERT_NE(&s1, &s2);
  ASSERT_EQ(s1, "def");
  s1 = "abc";
  ASSERT_EQ(s1, "abc");
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
