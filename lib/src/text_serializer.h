#pragma once

#include <prometheus/metric_registry.h>
#include <ostream>

namespace prometheus {
struct text_serializer {
  static void write(std::ostream &os, const metric_registry &registry);
};

} // namespace prometheus
