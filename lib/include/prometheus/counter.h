#pragma once

#include <prometheus/atomic_double.h>

namespace prometheus {

/**
 * metric type to track an increasing series of events
 */
class counter {
 public:

  inline void inc() {
    value += 1.0;
  }

  inline void inc(const double amount) {
    if (amount > 0.0) {
      value += amount;
    }
  }

  inline double val() const {
    return value.val();
  }

  inline void reset() {
    value = {};
  }

 private:
  detail::atomic_double value{};
};

} // namespace prometheus
