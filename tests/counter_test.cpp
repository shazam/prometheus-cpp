#include "prometheus/counter.h"

#include <gmock/gmock.h>

using namespace testing;
using namespace prometheus;

TEST(counter_test, initalizedToZero) {
  counter counter;
  ASSERT_THAT(counter.val(), DoubleEq(0.0));
}

TEST(counter_test, inc) {
  counter counter;
  counter.inc();
  ASSERT_THAT(counter.val(), DoubleEq(1.0));
}

TEST(counter_test, inc_number) {
  counter counter;
  counter.inc(4);
  ASSERT_THAT(counter.val(), DoubleEq(4.0));
}

TEST(counter_test
, inc_multiple) {
  counter counter;
  counter.inc();
  counter.inc();
  counter.inc(5);
  ASSERT_THAT(counter.val(), DoubleEq(7.0));
}
