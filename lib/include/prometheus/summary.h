#pragma once

#include "prometheus/atomic_double.h"
#include "prometheus/spin_lock.h"
#include "prometheus/metric_labels.h"
#include "prometheus/timers.h"

#include <type_traits>
#include <vector>
#include <chrono>

namespace prometheus {

inline bool summary_label_validator(const std::string &label) {
  const static std::string quantile_string("quantile");
  return label != quantile_string;
}

/**
 * summary metric type is used to track a series of events, and compute percentiles
 * of those events over a sliding window.
 *
 * Currently the only supported window type is a fixed series.
 *
 * https://prometheus.io/docs/practices/histograms/
 */
class summary {
 public:

  struct quantile {
    quantile(double quantile, double error);

    const double _quantile;
    const double error;
  };

  /**
   * Used to return an entire view of the summary
   * at a specific instance in time
   */
  struct snapshot {
    const std::vector<std::pair<double, double>> quantiles;
    const double sum;
    const uint64_t count;
  };

  using objectives = std::vector<quantile>;

  /**
   * New Summary metric that will be responsible for tracking the
   * most recent "window_size" calls to summary::observe
   * @param bounds The quantiles to compute for this metric
   * @param window_size How many observe calls to track
   */
  summary(summary::objectives bounds = {}, size_t window_size = 1000);

  /**
   * Use this value for computing the summary values
   * @param value metric to track
   */
  void observe(double value);

  inline bool empty() const {
    return current_index == 0;
  }

  /**
   * Helper function to time a function call
   * @param f The function to profile
   */
  template<typename Func>
  void observe_duration(Func f) {
    auto timer = make_scoped_timer(*this);
    f();
  }

  /**
   * @return The computed quantiles for this
   * specific moment in time
   */
  summary::snapshot get_snapshot() const;

  const objectives objecs;
  const uint64_t window_size;

  std::atomic<uint64_t> current_index{};
  std::vector<double> observed_values;
  mutable spin_lock read_write_lock;

};

} // namespace prometheus
