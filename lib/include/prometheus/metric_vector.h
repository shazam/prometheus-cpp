#pragma once

#include <prometheus/metric_family.h>
#include <prometheus/metric_labels.h>
#include <prometheus/check.h>

#include <type_traits>
#include <list>

namespace prometheus {

using label_validator = std::add_pointer<bool(const std::string &)>;

/**
 * Default label validator
 * always return true, allow any label name
 */
inline bool no_op(const std::string &) {
  return true;
}

template<typename T, label_validator::type F = no_op>
class metric_vector {

 public:

  metric_vector(const metric_family &family) : family(family) {
  }

  template<typename... Args>
  T &add(metric_labels labels, Args &&... args) {
    metrics.emplace_back(std::forward<Args>(args)...);
    for (const auto &values : labels) {
      (void) values;
      PROMETHEUS_CHECK(F(values.first));
    }

    for (const auto &values : family.labels) {
      (void) values;
      PROMETHEUS_CHECK(F(values.first));
    }
    metrics.back().labels = std::move(labels);
    return metrics.back();
  }

  template<typename... Args>
  T &add() {
    metrics.emplace_back();
    return metrics.back();
  }

  const metric_family &family;
  std::list<T> metrics;
};

} // namespace prometheus