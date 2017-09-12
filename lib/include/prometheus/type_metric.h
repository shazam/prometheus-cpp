#pragma once

#include <prometheus/metric.h>

namespace prometheus {

template<typename T, typename M = metric>
struct type_metric : public T, public M {
  using T::T;
};

} // namespace prometheus