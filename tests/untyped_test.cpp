#include <gmock/gmock.h>

#include <prometheus/untyped.h>

using namespace testing;
using namespace prometheus;

TEST(untyped_test, initToZero) {
  untyped testGauge;
  ASSERT_THAT(testGauge.val(), DoubleEq(0.0));
}

TEST(untyped_test, inc) {
  untyped testGauge;
  testGauge.inc();
  ASSERT_THAT(testGauge.val(), DoubleEq(1.0));
}

TEST(untyped_test, inc_value) {
  untyped testGauge;
  testGauge.inc(4);
  ASSERT_THAT(testGauge.val(), DoubleEq(4.0));
}

TEST(untyped_test, inc_by_negative_value) {
  untyped testGauge;
  testGauge.inc(-4);
  ASSERT_THAT(testGauge.val(), DoubleEq(-4.0));
}

TEST(untyped_test, inc_multiple) {
  untyped testGauge;
  testGauge.inc();
  testGauge.inc();
  testGauge.inc(5);
  ASSERT_THAT(testGauge.val(), DoubleEq(7.0));
}

TEST(untyped_test, dec) {
  untyped testGauge;
  testGauge.set(5.0);
  testGauge.dec();
  ASSERT_THAT(testGauge.val(), DoubleEq(4.0));
}

TEST(untyped_test, dec_number) {
  untyped testGauge;
  testGauge.set(5.0);
  testGauge.dec(3.0);
  ASSERT_THAT(testGauge.val(), DoubleEq(2.0));
}

TEST(untyped_test, set) {
  untyped testGauge;
  testGauge.set(3.0);
  ASSERT_THAT(testGauge.val(), DoubleEq(3.0));
}

TEST(untyped_test, set_multiple) {
  untyped testGauge;
  testGauge.set(3.0);
  testGauge.set(8.0);
  ASSERT_THAT(testGauge.val(), DoubleEq(8.0));
}

TEST(untyped_test, set_to_current_time) {
  untyped testGauge;
  testGauge.set_to_current_time();
  ASSERT_THAT(testGauge.val(), Gt(0.0));
}
