#include "gtest/gtest.h"

#include <chrono>
#include <condition_variable>

#include "gallocy/consensus/timer.h"


// TODO(sholsapp): Does this need to protect against spurious wake ups?
TEST(ConsensusTimerTests, StartStopReset) {
  std::condition_variable alarm_triggered;
  std::cv_status status;
  std::mutex m;
  uint64_t step = 100;

  Timer t(step, 0, std::addressof(alarm_triggered));

  // Start the timer
  t.start();

  {
    // Wait for alarm_triggered for twice as long as the step time (i.e., so it
    // is guaranteed to have gone off).
    std::unique_lock<std::mutex> lk(m);
    status = alarm_triggered.wait_for(lk, std::chrono::milliseconds(step * 2));
    ASSERT_EQ(status, std::cv_status::no_timeout);
  }

  // Reset the timer
  t.reset();

  {
    // Wait for alarm_triggered for half as long as the step time (i.e., so it
    // is guaranteed to have not gone off).
    std::unique_lock<std::mutex> lk(m);
    status = alarm_triggered.wait_for(lk, std::chrono::milliseconds(step / 2));
    ASSERT_EQ(status, std::cv_status::timeout);
  }

  // Reset the timer
  t.reset();

  {
    // Wait for alarm_triggered for twice as long as the step time (i.e., so it
    // is guaranteed to have gone off).
    std::unique_lock<std::mutex> lk(m);
    status = alarm_triggered.wait_for(lk, std::chrono::milliseconds(step * 2));
    ASSERT_EQ(status, std::cv_status::no_timeout);
  }

  // Stop the timer
  t.stop();
}
