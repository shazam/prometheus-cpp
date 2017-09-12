#include <gmock/gmock.h>

#include <prometheus/gauge.h>

using namespace testing;
using namespace prometheus;

TEST(gauge_test, initToZero) {
  gauge testGauge;
  ASSERT_THAT(testGauge.val(), DoubleEq(0.0));
}

TEST(gauge_test, inc) {
  gauge testGauge;
  testGauge.inc();
  ASSERT_THAT(testGauge.val(), DoubleEq(1.0));
}

TEST(gauge_test, inc_value) {
  gauge testGauge;
  testGauge.inc(4);
  ASSERT_THAT(testGauge.val(), DoubleEq(4.0));
}

TEST(gauge_test, inc_multiple) {
  gauge testGauge;
  testGauge.inc();
  testGauge.inc();
  testGauge.inc(5);
  ASSERT_THAT(testGauge.val(), DoubleEq(7.0));
}

TEST(gauge_test, dec) {
  gauge testGauge;
  testGauge.set(5.0);
  testGauge.dec();
  ASSERT_THAT(testGauge.val(), DoubleEq(4.0));
}

TEST(gauge_test, dec_number) {
  gauge testGauge;
  testGauge.set(5.0);
  testGauge.dec(3.0);
  ASSERT_THAT(testGauge.val(), DoubleEq(2.0));
}

TEST(gauge_test, set) {
  gauge testGauge;
  testGauge.set(3.0);
  ASSERT_THAT(testGauge.val(), DoubleEq(3.0));
}

TEST(gauge_test, set_multiple) {
  gauge testGauge;
  testGauge.set(3.0);
  testGauge.set(8.0);
  ASSERT_THAT(testGauge.val(), DoubleEq(8.0));
}

TEST(gauge_test, set_to_current_time) {
  gauge testGauge;
  testGauge.set_to_current_time();
  ASSERT_THAT(testGauge.val(), Gt(0.0));
}
