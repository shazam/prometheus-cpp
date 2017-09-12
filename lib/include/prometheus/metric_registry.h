#pragma once

#include <prometheus/gauge.h>
#include <prometheus/counter.h>
#include <prometheus/histogram.h>
#include <prometheus/summary.h>
#include <prometheus/metric_family.h>
#include <prometheus/type_metric.h>
#include <prometheus/metric_vector.h>

#include <list>
#include <mutex>

namespace prometheus {

using gauge_metric = type_metric<gauge>;
using counter_metric = type_metric<counter>;
using histogram_metric = type_metric<histogram>;
using summary_metric = type_metric<summary>;

using gauge_vector = metric_vector<gauge_metric>;
using counter_vector = metric_vector<counter_metric>;
using histogram_vector = metric_vector<histogram_metric, histogram_label_validator>;
using summary_vector = metric_vector<summary_metric, summary_label_validator>;

struct metric_registry {

  inline const metric_family &make_family(const metric_family::builder &builder) {
    std::lock_guard<std::mutex> guard(lock);
    families.emplace_back(builder);
    return families.back();
  }

  inline gauge_vector &make_gauge(const metric_family &family) {
    std::lock_guard<std::mutex> guard(lock);
    gauges.emplace_back(family);
    return gauges.back();
  }

  inline gauge_vector &make_gauge(metric_family::builder builder) {
    return make_gauge(make_family(builder));
  }

  inline counter_vector &make_counter(const metric_family &family) {
    std::lock_guard<std::mutex> guard(lock);
    counters.emplace_back(family);
    return counters.back();
  }

  inline counter_vector &make_counter(metric_family::builder builder) {
    return make_counter(make_family(builder));
  }

  inline histogram_vector &make_histogram(const metric_family &family) {
    std::lock_guard<std::mutex> guard(lock);
    histograms.emplace_back(family);
    return histograms.back();
  }

  inline histogram_vector &make_histogram(metric_family::builder builder) {
    return make_histogram(make_family(builder));
  }

  inline summary_vector &make_summary(const metric_family &family) {
    std::lock_guard<std::mutex> guard(lock);
    summaries.emplace_back(family);
    return summaries.back();
  }

  inline summary_vector &make_summary(metric_family::builder builder) {
    return make_summary(make_family(builder));
  }

  std::ostream &write(std::ostream &os) const;

  std::list<gauge_vector> gauges;
  std::list<counter_vector> counters;
  std::list<histogram_vector> histograms;
  std::list<summary_vector> summaries;

  std::list<metric_family> families;

  mutable std::mutex lock;
};

} // namespace prometheus