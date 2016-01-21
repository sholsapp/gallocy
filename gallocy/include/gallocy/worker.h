#ifndef GALLOCY_WORKER_H_
#define GALLOCY_WORKER_H_

#include <pthread.h>

#include "gallocy/threads.h"

/**
 * A gallocy daemon worker.
 *
 * A gallocy daemon worker enables a class to be "daemonized" by extending
 * this class and implementing a work function. A "daemonized" class is one
 * whose work function will be run in a gallocy threaded context.
 *
 * This class allows daemons to focus on the business logic, i.e., their work,
 * rather than thread plumbing.
 */
class ThreadedDaemon {
 public:
  /**
   * Start the daemon.
   */
  void start() {
    alive = true;
    if (__gallocy_pthread_create(&thread, nullptr, handle_work, reinterpret_cast<void *>(this))) {
      perror("pthread_create");
    }
  }
  /**
   * Stop the daemon.
   */
  void stop() {
    alive = false;
    if (__gallocy_pthread_join(thread, nullptr)) {
      perror("pthread_join");
    }
  }
  /**
   * A pthread invoker.
   */
  static void *handle_work(void *arg) {
    ThreadedDaemon *self = reinterpret_cast<ThreadedDaemon *>(arg);
    void *ret = self->work();
    return ret;
  }
  /**
   * The work function.
   *
   * The work function is a *pure abstract method* that must be implemented by
   * the descendant class. This method should continue running as long as
   * `alive` is true.
   */
  virtual void *work() = 0;
  /**
   * True if the daemon is alive.
   */
  bool alive;
 private:
  /**
   * The pthread identifier for this daemon.
   */
  pthread_t thread;
};

#endif  // GALLOCY_WORKER_H_
