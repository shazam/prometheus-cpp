#include <gmock/gmock.h>

#include "check_names.h"

using namespace testing;
using namespace prometheus;

TEST(check_names_test, empty_metric) {
  EXPECT_FALSE(valid_metric_name(""));
}

TEST(check_names_test, valid_metric) {
  EXPECT_TRUE(valid_metric_name("prometheus_notifications_total"));
}

TEST(check_names_test, reserved_metric) {
  EXPECT_FALSE(valid_metric_name("__some_reserved_metric"));
}

TEST(check_names_test, empty_label) {
  EXPECT_FALSE(valid_label_name(""));
}

TEST(check_names_test, valid_label) {
  EXPECT_TRUE(valid_label_name("type"));
}

TEST(check_names_test, reserved_label) {
  EXPECT_FALSE(valid_label_name("__some_reserved_label"));
}
