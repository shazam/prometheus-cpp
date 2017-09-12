#include <text_serializer.h>
#include <sstream>

#include <gmock/gmock.h>

using namespace prometheus;
using namespace testing;
using namespace std;

namespace {

const map<string, string> SOME_LABELS{
    {"label1", "value1"},
    {"label2", "value2"}
};

const summary::objectives SOME_OBJECTIVES{
    {.01, .001},
    {.05, .001},
    {.5, .001},
    {.90, .001},
    {.99, .001},
};
}

TEST(text_searializer_summary_test, with_help) {
  constexpr auto EXPECTED = R"(# HELP request_duration something
# TYPE request_duration summary
request_duration{quantile="0.01"} 1
request_duration{quantile="0.05"} 1
request_duration{quantile="0.5"} 1
request_duration{quantile="0.9"} 1
request_duration{quantile="0.99"} 1
request_duration_sum 1
request_duration_count 1

)";

  metric_registry reg;
  auto &request_duration_family = reg.make_summary(metric_family::builder("request_duration").with_help("something"));
  request_duration_family.add({}, SOME_OBJECTIVES).observe(1.0);

  stringstream str;
  text_serializer::write(str, reg);
  ASSERT_THAT(str.str(), StrEq(EXPECTED));
}

TEST(text_searializer_summary_test, with_observe) {
  constexpr auto EXPECTED = R"(# TYPE request_duration summary
request_duration{quantile="0.01"} 500
request_duration{quantile="0.05"} 500
request_duration{quantile="0.5"} 500
request_duration{quantile="0.9"} 500
request_duration{quantile="0.99"} 500
request_duration_sum 500
request_duration_count 1

)";

  metric_registry reg;
  auto &request_duration_family = reg.make_summary(metric_family::builder("request_duration"));
  auto &request_duration = request_duration_family.add({}, // no labels
                                                       SOME_OBJECTIVES, // "default" objectives
                                                       1); // number of items in sliding window

  request_duration.observe(500);

  stringstream str;
  text_serializer::write(str, reg);
  ASSERT_THAT(str.str(), StrEq(EXPECTED));
}

TEST(text_searializer_summary_test, with_labels) {
  constexpr auto EXPECTED = R"(# TYPE request_duration summary
request_duration{quantile="0.01",label1="value1",label2="value2",} 1
request_duration{quantile="0.05",label1="value1",label2="value2",} 1
request_duration{quantile="0.5",label1="value1",label2="value2",} 1
request_duration{quantile="0.9",label1="value1",label2="value2",} 1
request_duration{quantile="0.99",label1="value1",label2="value2",} 1
request_duration_sum{label1="value1",label2="value2",} 1
request_duration_count{label1="value1",label2="value2",} 1

)";

  metric_registry reg;
  auto &request_duration_family = reg.make_summary(metric_family::builder("request_duration"));
  request_duration_family.add(SOME_LABELS, SOME_OBJECTIVES).observe(1);

  stringstream str;
  text_serializer::write(str, reg);
  ASSERT_THAT(str.str(), StrEq(EXPECTED));
}

TEST(text_searializer_summary_test, multi_family_metric) {
  constexpr auto EXPECTED = R"(# TYPE request_duration summary
request_duration{quantile="0.01"} 1
request_duration{quantile="0.05"} 1
request_duration{quantile="0.5"} 1
request_duration{quantile="0.9"} 1
request_duration{quantile="0.99"} 1
request_duration_sum 1
request_duration_count 1
request_duration{quantile="0.01",label1="value1",label2="value2",} 1
request_duration{quantile="0.05",label1="value1",label2="value2",} 1
request_duration{quantile="0.5",label1="value1",label2="value2",} 1
request_duration{quantile="0.9",label1="value1",label2="value2",} 1
request_duration{quantile="0.99",label1="value1",label2="value2",} 1
request_duration_sum{label1="value1",label2="value2",} 1
request_duration_count{label1="value1",label2="value2",} 1

)";
  metric_registry reg;
  auto &request_duration_family = reg.make_summary(metric_family::builder("request_duration"));
  request_duration_family.add({}, SOME_OBJECTIVES).observe(1);
  request_duration_family.add(SOME_LABELS, SOME_OBJECTIVES).observe(1);

  stringstream str;
  text_serializer::write(str, reg);
  ASSERT_THAT(str.str(), StrEq(EXPECTED));
}


TEST(text_searializer_summary_test, skips_everything_if_empty) {
  constexpr auto EXPECTED = R"()";
  metric_registry reg;
  auto &request_duration_family = reg.make_summary(metric_family::builder("request_duration"));
  request_duration_family.add({}, SOME_OBJECTIVES);
  request_duration_family.add(SOME_LABELS, SOME_OBJECTIVES);

  stringstream str;
  text_serializer::write(str, reg);
  ASSERT_THAT(str.str(), StrEq(EXPECTED));
}