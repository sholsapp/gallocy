#include <chrono>
#include <condition_variable>
#include <iostream>
#include <thread>

class Timer {
 public:
  /**
   * Construct a timer.
   */
  Timer(uint64_t step, std::condition_variable *timed_out) {
    _step = step;
    _timed_out = timed_out;
  }
  Timer(const Timer &other) = delete;
  Timer &operator=(Timer &other) = delete;
  /**
   * Start the timer.
   */
  void start() {
    is_alive = true;
    t = std::move(std::thread([this]() { wait(); }));
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

 private:
  /**
   * The timer event loop.
   */
  void wait() {
    while (is_alive) {
      std::unique_lock<std::mutex> lk(cv_m);
      std::cv_status status = cv.wait_for(lk, std::chrono::milliseconds(_step));
      if (status == std::cv_status::timeout) {
        std::cout << "> TIMEOUT" << std::endl;
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
  std::condition_variable *_timed_out;
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
  uint64_t _step;
};

