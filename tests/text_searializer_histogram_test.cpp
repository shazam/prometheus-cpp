#include <text_serializer.h>
#include <sstream>

#include <gmock/gmock.h>

using namespace prometheus;
using namespace testing;
using namespace std;

namespace {

const map<string, string> SOME_LABELS = {
    {"label1", "value1"},
    {"label2", "value2"}
};

}

TEST(text_searializer_histogram_test, with_help) {
  constexpr auto EXPECTED = R"(# HELP total_count something
# TYPE total_count histogram
total_count_bucket{le="0.05"} 1
total_count_bucket{le="0.1"} 1
total_count_bucket{le="0.2"} 1
total_count_bucket{le="0.5"} 1
total_count_bucket{le="1"} 2
total_count_bucket{le="+Inf"} 3
total_count_sum 3.05
total_count_count 3

)";

  metric_registry reg;
  auto &total_counter_family = reg.make_histogram(metric_family::builder("total_count").with_help("something"));
  auto &total_counter = total_counter_family.add({}, custom_bounds(0.05, 0.1, 0.2, 0.5, 1.0));
  total_counter.observe(.05);
  total_counter.observe(1.);
  total_counter.observe(2.);

  stringstream str;
  text_serializer::write(str, reg);
  ASSERT_THAT(str.str(), StrEq(EXPECTED));
}

TEST(text_searializer_histogram_test, without_help_no_buckets) {
  constexpr auto EXPECTED = R"(# TYPE total_count histogram
total_count_bucket{le="+Inf"} 1
total_count_sum 2
total_count_count 1

)";

  metric_registry reg;
  auto &total_counter_family = reg.make_histogram(metric_family::builder("total_count"));
  auto &total_counter = total_counter_family.add({}, custom_bounds());
  total_counter.observe(2.);

  stringstream str;
  text_serializer::write(str, reg);
  ASSERT_THAT(str.str(), StrEq(EXPECTED));
}

TEST(text_searializer_histogram_test, with_labels) {
  constexpr auto EXPECTED = R"(# TYPE total_count histogram
total_count_bucket{le="0.5",label1="value1",label2="value2",} 1
total_count_bucket{le="+Inf",label1="value1",label2="value2",} 1
total_count_sum{label1="value1",label2="value2",} 0.5
total_count_count{label1="value1",label2="value2",} 1

)";

  metric_registry reg;
  auto &total_counter_family = reg.make_histogram(metric_family::builder("total_count").with_labels(SOME_LABELS));
  auto &total_counter = total_counter_family.add({}, custom_bounds(.5));
  total_counter.observe(0.5);

  stringstream str;
  text_serializer::write(str, reg);
  ASSERT_THAT(str.str(), StrEq(EXPECTED));
}

TEST(text_searializer_histogram_test, multi_family_metric) {
  constexpr auto EXPECTED = R"(# TYPE total_count histogram
total_count_bucket{le="0.5"} 1
total_count_bucket{le="+Inf"} 1
total_count_sum 0.5
total_count_count 1
total_count_bucket{le="0.6",label1="value1",label2="value2",} 0
total_count_bucket{le="+Inf",label1="value1",label2="value2",} 1
total_count_sum{label1="value1",label2="value2",} 0.7
total_count_count{label1="value1",label2="value2",} 1

)";
  metric_registry reg;

  auto &total_counter_family = reg.make_histogram(metric_family::builder("total_count"));

  auto &total_counter0 = total_counter_family.add({}, custom_bounds(.5));
  total_counter0.observe(0.5);

  auto &total_counter1 = total_counter_family.add(SOME_LABELS, custom_bounds(.6));
  total_counter1.observe(0.7);

  stringstream str;
  text_serializer::write(str, reg);
  ASSERT_THAT(str.str(), StrEq(EXPECTED));
}


TEST(text_searializer_histogram_test, nothing_all_empty) {
  constexpr auto EXPECTED = R"()";
  metric_registry reg;

  auto &total_counter_family = reg.make_histogram(metric_family::builder("total_count"));
  total_counter_family.add({}, custom_bounds(.5));

  stringstream str;
  text_serializer::write(str, reg);
  ASSERT_THAT(str.str(), StrEq(EXPECTED));
}


TEST(text_searializer_histogram_test, skips_empty) {
  constexpr auto EXPECTED = R"(# TYPE total_count histogram
total_count_bucket{le="0.5",label1="value1",label2="value2",} 1
total_count_bucket{le="+Inf",label1="value1",label2="value2",} 1
total_count_sum{label1="value1",label2="value2",} 0.5
total_count_count{label1="value1",label2="value2",} 1

)";
  metric_registry reg;

  auto &total_counter_family = reg.make_histogram(metric_family::builder("total_count"));
  total_counter_family.add({}, custom_bounds(.5));
  auto &total_counter = total_counter_family.add(SOME_LABELS, custom_bounds(.5));
  total_counter.observe(0.5);

  stringstream str;
  text_serializer::write(str, reg);
  ASSERT_THAT(str.str(), StrEq(EXPECTED));
}