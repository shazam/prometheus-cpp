#pragma once

#include "prometheus/atomic_double.h"
#include <prometheus/timers.h>
#include <string>
#include <vector>

#include <algorithm>
#include <cassert>

namespace prometheus {

inline bool histogram_label_validator(const std::string &label) {
  const static std::string le_string("le");
  return label != le_string;
}

/**
 * metric type to track a range of values.
 *
 * This will gather all values into buckets specific by histogram::bounds
 * and will allow aggregation and averages over different sources.
 *
 * https://prometheus.io/docs/practices/histograms/
 */
struct histogram {
  using bounds = std::vector<double>;

  histogram(histogram::bounds bounds);

  void observe(double value);

  inline bool empty() const {
    return count == 0;
  }

  template<typename Func>
  void observe_duration(Func f) {
    auto timer = make_scoped_timer(*this);
    f();
  }

  detail::atomic_double sum;
  const bounds boundaries;
  std::vector<std::atomic<uint64_t>> counts;
  std::atomic<uint64_t> count{};
};

histogram::bounds linear_bounds(double start, double width, size_t count);
histogram::bounds exponential_bounds(double start, double factor, size_t count);

template<typename... Args>
histogram::bounds custom_bounds(Args &&... args) {
  return histogram::bounds{std::forward<double>(args)...};
}

} // namespace prometheus
