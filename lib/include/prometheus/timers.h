#pragma once

#include <chrono>

namespace prometheus {

using timer_clock = std::chrono::high_resolution_clock;
using timer_duration = timer_clock::duration;
using timer_point = timer_clock::time_point;

/**
 * Used to profile an event or section of code
 */
template<typename T>
class timer {
 public:

  explicit timer(T &listener, timer_duration start) : listener(listener), start(start) {
  }

  /**
 * compute the delta from when the timer was created and osverve it
 */
  inline void observe_current_duration() {
    // convert to fractional seconds
    const std::chrono::duration<double> dsec = timer_clock::now() - start;
    listener.observe(dsec.count());
  }

 protected:
  T &listener;
  const timer_point start;
};

template<typename T>
timer<T> make_timer(T &observer, const timer_duration &start = timer_clock::now().time_since_epoch()) {
  return timer<T>(observer, start);
}

/**
 * Used to profile an event or section of code
 *
 * This will automatically observe the duration on destruction
 *
 * ex:
 * {
 *      scoped_timer<T> timer;
 *      // some code
 * } // <= timer will observe here
 */
template<typename T>
class scoped_timer : private timer<T> {
 public:
  using timer<T>::timer;

  ~scoped_timer() {
    timer<T>::observe_current_duration();
  }
};

template<typename T>
scoped_timer<T> make_scoped_timer(T &observer, const timer_duration &start = timer_clock::now().time_since_epoch()) {
  return scoped_timer<T>(observer, start);

}

} // namespace prometheus