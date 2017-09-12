#pragma once

#include <prometheus/metric_labels.h>

namespace prometheus {

struct metric {
  metric_labels labels;

  std::chrono::milliseconds timestamp;

  inline void set_timestamp_now() {
    using namespace std::chrono;
    timestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
  }

  inline void set_timestamp(std::chrono::milliseconds time) {
    timestamp = time;
  }
};

} // namespace prometheus