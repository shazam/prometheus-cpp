#include <text_serializer.h>
#include <prometheus/metric_registry.h>
#include <sstream>

#include <gmock/gmock.h>

using namespace prometheus;
using namespace testing;

TEST(text_searializer_gauge_test, without_help) {
  constexpr auto EXPECTED = R"(# TYPE total_count gauge
total_count 2.5

)";

  metric_registry reg;
  auto &total_counter_family = reg.make_gauge(metric_family::builder("total_count"));
  auto &total_counter = total_counter_family.add();
  total_counter.inc(2.5);

  std::stringstream str;
  text_serializer::write(str, reg);
  ASSERT_THAT(str.str(), StrEq(EXPECTED));
}

TEST(text_searializer_gauge_test, with_family) {
  constexpr auto EXPECTED = R"(# HELP total_count Total Request count.
# TYPE total_count gauge
total_count 1

)";

  metric_registry reg;

  auto tag_family = reg.make_family(metric_family::builder("total_count").with_help("Total Request count."));
  auto &total_counter_family = reg.make_gauge(tag_family);
  auto &total_counter = total_counter_family.add();

  total_counter.inc();

  std::stringstream str;
  text_serializer::write(str, reg);

  ASSERT_THAT(str.str(), StrEq(EXPECTED));
}

