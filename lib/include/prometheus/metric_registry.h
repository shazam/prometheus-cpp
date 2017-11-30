#pragma once

#include <prometheus/untyped.h>
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

using untyped_metric = type_metric<untyped>;
using gauge_metric = type_metric<gauge>;
using counter_metric = type_metric<counter>;
using histogram_metric = type_metric<histogram>;
using summary_metric = type_metric<summary>;

using untyped_vector = metric_vector<untyped_metric>;
using gauge_vector = metric_vector<gauge_metric>;
using counter_vector = metric_vector<counter_metric>;
using histogram_vector = metric_vector<histogram_metric, histogram_label_validator>;
using summary_vector = metric_vector<summary_metric, summary_label_validator>;

class metric_registry {
  template<class T, class Group>
  inline T &make_metric_t(const metric_family &family, Group &group) {
    std::lock_guard<std::mutex> guard(lock);

    // Check to see if metric with same name has been added to group already
    for (const auto &item : group) {
      (void) item;
      PROMETHEUS_CHECK(item.family.name == family.name);
    }

    group.emplace_back(family);
    return group.back();
  }

 public:
  inline const metric_family &make_family(const metric_family::builder &builder) {
    std::lock_guard<std::mutex> guard(lock);
    families.emplace_back(builder);
    return families.back();
  }

  inline gauge_vector &make_gauge(const metric_family &family) {
    return make_metric_t<gauge_vector>(family, gauges);
  }

  inline gauge_vector &make_gauge(const metric_family::builder &builder) {
    return make_gauge(make_family(builder));
  }

  inline counter_vector &make_counter(const metric_family &family) {
    return make_metric_t<counter_vector>(family, counters);
  }

  inline counter_vector &make_counter(const metric_family::builder &builder) {
    return make_counter(make_family(builder));
  }

  inline histogram_vector &make_histogram(const metric_family &family) {
    return make_metric_t<histogram_vector>(family, histograms);
  }

  inline histogram_vector &make_histogram(const metric_family::builder &builder) {
    return make_histogram(make_family(builder));
  }

  inline summary_vector &make_summary(const metric_family &family) {
    return make_metric_t<summary_vector>(family, summaries);
  }

  inline summary_vector &make_summary(const metric_family::builder &builder) {
    return make_summary(make_family(builder));
  }

  inline untyped_vector &make_untyped(const metric_family &family) {
    return make_metric_t<untyped_vector>(family, untyped);
  }

  inline untyped_vector &make_untyped(const metric_family::builder &builder) {
    return make_untyped(make_family(builder));
  }

  /**
   * Write out all registred metric as the prometheus text format
   * @param os Stream to write to.
   */
  void write(std::ostream &os) const;

  std::list<gauge_vector> gauges;
  std::list<counter_vector> counters;
  std::list<histogram_vector> histograms;
  std::list<summary_vector> summaries;
  std::list<untyped_vector> untyped;

  std::list<metric_family> families;

  // make sure only one person at a time modifies the registry
  mutable std::mutex lock;
};

} // namespace prometheus