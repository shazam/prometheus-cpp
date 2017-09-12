#include <prometheus/summary.h>
#include <prometheus/check.h>

#include <numeric>
#include <algorithm>
#include <cstring>

using namespace std;

namespace prometheus {

summary::quantile::quantile(const double quantile, const double error) : _quantile(quantile),
                                                                         error(error) {
  PROMETHEUS_CHECK(quantile >= 0.0 && quantile <= 1.0);
  PROMETHEUS_CHECK(error >= 0.0 && error <= 1.0);
}

summary::summary(summary::objectives objectives, const size_t window_size) : objecs(move(objectives)),
                                                                             window_size(window_size),
                                                                             observed_values(window_size) {
  PROMETHEUS_CHECK(is_sorted(begin(objecs), end(objecs), [](const quantile &l, const quantile &r) {
    return l._quantile < r._quantile;
  }));
}

void summary::observe(const double value) {

  // TODO: double buffer the writes, so that we dont block
  read_write_lock.lock();

  // we want to read current_index before we increment it
  // the rmw is ATOMIC so multiple people writing is fine.
  const uint64_t current_write_index = current_index++ % window_size;
  observed_values[current_write_index] = value;

  read_write_lock.unlock();
}

summary::snapshot summary::get_snapshot() const {

  // temp buffer to store the current data into and then
  // used to compact everything into the quantile ranges
  // TODO: we could reuse this buffer
  vector<double> shadow_values(window_size);
  uint64_t shadow_index{};

  read_write_lock.lock();
  {
    // !! Only read once we have the lock!!!
    shadow_index = current_index;
    memcpy(shadow_values.data(), observed_values.data(), observed_values.size() * sizeof(observed_values.back()));
  }
  read_write_lock.unlock();

  // now we have a copy so we can be certain that no one is going to change
  // anything out from under us

  vector<pair<double, double>> quantiles;
  quantiles.reserve(objecs.size());

  // if the buffer is not full we dont want to use the out of
  // range default values
  if (shadow_index > shadow_values.size()) {
    shadow_index = shadow_values.size();
  }

  for (auto &objective : objecs) {
    const auto quantile_index = static_cast<size_t>(shadow_index * objective._quantile);

    nth_element(shadow_values.begin(),
                shadow_values.begin() + quantile_index,
                shadow_values.begin() + shadow_index);

    quantiles.emplace_back(objective._quantile,
                           *(shadow_values.begin() + quantile_index));
  }

  return {
      quantiles,
      accumulate(shadow_values.begin(), shadow_values.begin() + shadow_index, 0.0),
      shadow_index
  };
}

} // namespace prometheus
