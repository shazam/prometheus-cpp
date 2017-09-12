#pragma once

#include <atomic>
#include <functional>

namespace prometheus {

namespace detail {

/**
 * the C++ standard does not overload operators for double/float atomic
 * types, so we will do that ourself.
 */
class atomic_double {
 public:

  atomic_double() = default;

  inline atomic_double(const double value) : value(value) {
  }

  inline atomic_double &operator+=(const double rhs) {
    return atomic_op(rhs, std::plus<double>());
  }

  inline atomic_double &operator-=(const double rhs) {
    return atomic_op(rhs, std::minus<double>());
  }

  inline double val() const {
    return value.load();
  }

  inline atomic_double &operator=(const double desired) {
    atomic_op(desired, [](double, double desired) { return desired; });
    return *this;
  }

 private:

  template<typename BinaryOp>
  atomic_double &atomic_op(double d, BinaryOp _Op) {
    using namespace std;

    auto old = val();
    auto desired = _Op(old, d);
    while (!value.compare_exchange_weak(old, desired)) {
      desired = _Op(old, d);
    }

    return *this;
  }

  std::atomic<double> value{};
};

} // namespace detail
} // namespace prometheus