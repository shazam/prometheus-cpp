#pragma once

#include <prometheus/metric_labels.h>

namespace prometheus {

struct metric {
  metric_labels labels;
  bool active = true;

  inline void disable() { active = false; }
  inline void enable() { active = true; }

};

} // namespace prometheus