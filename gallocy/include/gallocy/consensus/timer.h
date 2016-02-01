#ifndef GALLOCY_CONSENSUS_TIMER_H_
#define GALLOCY_CONSENSUS_TIMER_H_

#include <chrono>
#include <condition_variable>
#include <ctime>
#include <iostream>
#include <thread>

class Timer {
 public:
  /**
   * Construct a timer.
   */
  Timer(uint64_t step, uint64_t jitter, std::condition_variable *timed_out) :
    timed_out(timed_out),
    step(step),
    jitter(jitter)
  {}
  Timer(const Timer &other) = delete;
  Timer &operator=(Timer &other) = delete;
  /**
   * Start the timer.
   */
  void start() {
    is_alive = true;
    t = std::move(std::thread([this]() { event_loop(); }));
  }
  /**
   * Stop the timer.
   */
  void stop() {
    is_alive = false;
    cv.notify_all();
    t.join();
  }
  /**
   * Reset the timer.
   */
  void reset() {
    was_reset = true;
    cv.notify_all();
  }
  /**
   * Set a new step time.
   *
   * :param new_step: The new step time in milliseconds.
   */
  void set_step(uint64_t new_step) {
    step = new_step;
  }
  /**
   * True if the timer is currently running.
   */
  bool is_timer_running() {
    return is_alive;
  }

 private:
  /**
   * The timer event loop.
   */
  void event_loop() {
    while (is_alive) {
      std::unique_lock<std::mutex> lk(cv_m);
      std::cv_status status = cv.wait_for(lk, calculate_wait_time());
      if (status == std::cv_status::timeout) {
        std::cout << "> TIMEOUT" << std::endl;
        timed_out->notify_all();
      } else if (status == std::cv_status::no_timeout) {
        if (was_reset) {
          was_reset = false;
          std::cout << "> RESET" << std::endl;
        } else {
          std::cout << "> SIGNALED" << std::endl;
        }
      }
    }
  }
  /**
   * Calculate a random wait time.
   *
   * The random wait time calculated is somewhere between `step` and `step /
   * 2`, and should be suitable in a Raft consensus protocol.
   */
  std::chrono::milliseconds calculate_wait_time() {
    return std::chrono::milliseconds(step - std::rand() % (step / 2));
  }

 private:
  /**
   * True if the timer event loop should continue running.
   */
  bool is_alive;
  /**
   * True if the timer was reset.
   */
  bool was_reset;
  /**
   * Condition signaled if/when a time out occurs.
   */
  std::condition_variable *timed_out;
  /**
   * Condition used internally for starting, stopping, and resetting the timer.
   */
  std::condition_variable cv;
  /**
   * Mutex used internally for accessing internal condition `cv`.
   */
  std::mutex cv_m;
  /**
   * Thread used internally for the timer.
   */
  std::thread t;
  /**
   * The amount of time in milliseconds to use as a countdown constant.
   */
  uint64_t step;
  /**
   * The amount of time in milliseconds to use as an upper bound for jitter.
   */
  uint64_t jitter;
};

#endif  // GALLOCY_CONSENSUS_TIMER_H_
