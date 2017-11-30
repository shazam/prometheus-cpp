#pragma once

#include <prometheus/gauge.h>

namespace prometheus {

/**
 * metric type to track a "measurement"
 */
class untyped : private gauge {
 public:
  using gauge::gauge;
  using gauge::inc;
  using gauge::dec;
  using gauge::set;
  using gauge::val;
  using gauge::set_to_current_time;
};

} // namespace prometheus
