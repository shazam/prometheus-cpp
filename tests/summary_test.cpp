#include <prometheus/summary.h>
#include <thread>
#include <gmock/gmock.h>

using namespace std;
using namespace std::chrono;
using namespace testing;
using namespace prometheus;

namespace {

const summary::objectives SOME_OBJECTIVES{
    {0.5, 0.05},
    {0.9, 0.01},
    {0.99, 0.001}
};

} // namespace

TEST(summary_test, initialize_with_zero) {
  summary sumry;

  auto snap = sumry.get_snapshot();

  ASSERT_THAT(snap.sum, DoubleEq(0.0));
  ASSERT_THAT(snap.count, Eq(0));
}

TEST(summary_test, observer_duration) {
  summary sumry;
  sumry.observe_duration([]() {
    this_thread::sleep_for(milliseconds(1));
  });
  auto snap = sumry.get_snapshot();
  ASSERT_THAT(snap.count, Eq(1));
}

TEST(summary_test, make_timer) {
  summary sumry;

  auto timer = make_timer(sumry);
  {
    this_thread::sleep_for(milliseconds(1));
  }
  timer.observe_current_duration();

  auto snap = sumry.get_snapshot();
  ASSERT_THAT(snap.count, Eq(1));
}

TEST(summary_test, make_scoped_timer) {
  summary sumry;

  {
    auto timer = make_scoped_timer(sumry);
    this_thread::sleep_for(milliseconds(1));
  }

  auto snap = sumry.get_snapshot();
  ASSERT_THAT(snap.count, Eq(1));
}

TEST(summary_test, observe) {
  constexpr size_t nSamples = 1000000;

  summary sumry(SOME_OBJECTIVES, nSamples);

  // simulate one million samples
  for (uint64_t i = 1; i < nSamples; ++i) {
    sumry.observe(i);
  }

  const auto snap = sumry.get_snapshot();

  ASSERT_THAT(snap.sum, DoubleEq(499999500000.0));
  ASSERT_THAT(snap.count, Eq(999999));

  ASSERT_THAT(snap.quantiles, SizeIs(3));
  ASSERT_THAT(snap.quantiles[0], Pair(0.5, DoubleNear(0.5 * nSamples, 0.05 * nSamples)));
  ASSERT_THAT(snap.quantiles[1], Pair(0.9, DoubleNear(0.9 * nSamples, 0.01 * nSamples)));
  ASSERT_THAT(snap.quantiles[2], Pair(0.99, DoubleNear(0.99 * nSamples, 0.001 * nSamples)));
}

TEST(summary_test, sliding_windows) {
  constexpr size_t nSamples = 10;

  summary sumry(SOME_OBJECTIVES, nSamples);


  // set everything to a giant number
  for (uint64_t i = 0; i < nSamples; ++i) {
    sumry.observe(100000);
  }

  // 1 -> 10
  for (uint64_t i = 1; i < nSamples + 1; ++i) {
    sumry.observe(i);
  }

  const auto snap = sumry.get_snapshot();

  ASSERT_THAT(snap.sum, DoubleEq(55));
  ASSERT_THAT(snap.count, Eq(10));

  ASSERT_THAT(snap.quantiles, SizeIs(3));
  ASSERT_THAT(snap.quantiles[0], Pair(0.5, DoubleNear(6, .01)));
  ASSERT_THAT(snap.quantiles[1], Pair(0.9, DoubleNear(10, 0.01)));
  ASSERT_THAT(snap.quantiles[2], Pair(0.99, DoubleNear(10, 0.01)));
}

TEST(summary_test, non_full_window) {
  constexpr size_t nSamples = 100000;

  summary sumry(SOME_OBJECTIVES, nSamples);

  // 1 -> 10
  for (uint64_t i = 1; i < 11; ++i) {
    sumry.observe(i);
  }

  const auto snap = sumry.get_snapshot();

  ASSERT_THAT(snap.sum, DoubleEq(55));
  ASSERT_THAT(snap.count, Eq(10));

  ASSERT_THAT(snap.quantiles, SizeIs(3));
  ASSERT_THAT(snap.quantiles[0], Pair(0.5, DoubleNear(6, .01)));
  ASSERT_THAT(snap.quantiles[1], Pair(0.9, DoubleNear(10, 0.01)));
  ASSERT_THAT(snap.quantiles[2], Pair(0.99, DoubleNear(10, 0.01)));
}