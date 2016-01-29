#include "gtest/gtest.h"

#include <chrono>
#include <condition_variable>

#include "gallocy/consensus/timer.h"


TEST(ConsensusTimerTests, StartStopReset) {
  std::condition_variable test;
  Timer t(100, std::addressof(test));
  t.start();
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  t.reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  t.stop();
  ASSERT_TRUE(true);
}
