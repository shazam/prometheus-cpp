#pragma once

#include <chrono>
#include <prometheus/atomic_double.h>

namespace prometheus {

/**
 * metric type to track a "measurement"
 */
class gauge {
 public:

  gauge() = default;

  inline gauge(double value) : value{value} {
  }

  inline void inc() {
    value += 1.0;
  }

  inline void inc(const double value) {
    if (value > 0.0) {
      this->value += value;
    }
  }

  inline void dec() {
    value -= 1.0;
  }

  inline void dec(const double value) {
    if (value > 0.0) {
      this->value -= value;
    }
  }

  inline void set(const double newValue) {
    value = newValue;
  }

  inline void set_to_current_time() {
    using namespace std::chrono;
    const duration<double> time = system_clock::now().time_since_epoch();
    value = time.count();
  }

  inline double val() const {
    return value.val();
  }

 private:
  detail::atomic_double value{};
};

} // namespace prometheus
