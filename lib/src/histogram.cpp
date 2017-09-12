#include <prometheus/histogram.h>
#include <prometheus/check.h>

#include <functional>

using namespace std;

namespace {

vector<double> &addPosInf(vector<double> &bounds) {
  bounds.emplace_back(numeric_limits<double>::infinity());
  return bounds;
}

template<typename Func>
prometheus::histogram::bounds make_bounds(Func func, double start, const double stride, const size_t count) {
  prometheus::histogram::bounds bounds(count);

  for (size_t i = 0; i < count; ++i) {
    bounds[i] = start;
    start = func(start, stride);
  }

  return bounds;
}

} // namespace

namespace prometheus {

histogram::histogram(histogram::bounds bounds) : boundaries(move(addPosInf(bounds))),
                                                 counts(boundaries.size()) {
  PROMETHEUS_CHECK(is_sorted(begin(boundaries), end(boundaries)));
}

void histogram::observe(const double value) {
  sum += value;
  ++count;

  auto bucket_index =
      static_cast<size_t>(distance(
          boundaries.begin(),
          lower_bound(begin(boundaries), end(boundaries), value)
      ));

  ++counts[bucket_index];
}

histogram::bounds linear_bounds(const double start, const double width, const size_t count) {
  PROMETHEUS_CHECK(start >= 0);
  PROMETHEUS_CHECK(width > 0);
  PROMETHEUS_CHECK(count > 0);
  return make_bounds(std::plus<double>(), start, width, count);
}

histogram::bounds exponential_bounds(const double start, const double factor, const size_t count) {
  PROMETHEUS_CHECK(start >= 0);
  PROMETHEUS_CHECK(factor > 1);
  PROMETHEUS_CHECK(count > 0);
  return make_bounds(std::multiplies<double>(), start, factor, count);
}

} // namespace prometheus
