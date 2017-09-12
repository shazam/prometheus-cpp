#include <limits>

#include <gmock/gmock.h>

#include <prometheus/histogram.h>

using namespace testing;
using namespace prometheus;

TEST(histogram_test, initialize_with_zero) {
  histogram histogram({});
  ASSERT_THAT(histogram.sum.val(), DoubleEq(0.0));
  ASSERT_THAT(histogram.boundaries, SizeIs(1));
  ASSERT_THAT(histogram.counts, SizeIs(1));
}

TEST(histogram_test, sample_count) {
  histogram histogram({1});
  histogram.observe(0);
  histogram.observe(200);
  ASSERT_THAT(histogram.sum.val(), DoubleEq(200.0));
}

TEST(histogram_test, bucket_bounds) {
  std::vector<double> EXPECTED_BOUNDS{1, 2, std::numeric_limits<double>::infinity()};

  histogram histogram{histogram::bounds{1, 2}};
  ASSERT_THAT(histogram.counts, SizeIs(3));
  ASSERT_THAT(histogram.boundaries, ContainerEq(EXPECTED_BOUNDS));
}

TEST(histogram_test, observe) {
  histogram histogram{histogram::bounds{1, 2}};
  histogram.observe(1.5);
  ASSERT_THAT(histogram.counts[0], Eq(0));
  ASSERT_THAT(histogram.counts[1], Eq(1));
  ASSERT_THAT(histogram.counts[2], Eq(0));
}

TEST(histogram_test, linear_bounds_one_stride) {
  std::vector<double> EXPECTED{1, 2, 3, 4, 5};
  ASSERT_THAT(linear_bounds(1, 1, 5), ContainerEq(EXPECTED));
}

TEST(histogram_test, linear_bounds_three_stride) {
  std::vector<double> EXPECTED{0, 3, 6, 9};
  ASSERT_THAT(linear_bounds(0, 3, 4), ContainerEq(EXPECTED));
}

TEST(histogram_test, exp_bounds) {
  std::vector<double> EXPECTED{100, 120, 144};
  ASSERT_THAT(exponential_bounds(100, 1.2, 3), ContainerEq(EXPECTED));
}

TEST(histogram_test, timer) {
  histogram histogram{linear_bounds(0, 100000, 2)};
  {
    // This is going to set the begin time to Jan 1st 1970
    // the delta observed is going to be GIANT!
    make_scoped_timer(histogram, {std::chrono::seconds(0)});
  }
  ASSERT_THAT(histogram.counts[0], Eq(0));
  ASSERT_THAT(histogram.counts[1], Eq(0));
  ASSERT_THAT(histogram.counts[2], Eq(1));
}