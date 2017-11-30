#include <text_serializer.h>
#include <prometheus/metric_registry.h>
#include <sstream>

#include <gmock/gmock.h>

using namespace prometheus;
using namespace testing;

TEST(text_searializer_untyped_test, without_family) {
  constexpr auto EXPECTED = R"(# HELP total_count Total Request count.
# TYPE total_count untyped
total_count 2.5

)";

  metric_registry reg;
  auto &total_untyped_family = reg.make_untyped(metric_family::builder("total_count", "Total Request count."));
  auto &total_counter = total_untyped_family.add();
  total_counter.inc(2.5);

  std::stringstream str;
  text_serializer::write(str, reg);
  ASSERT_THAT(str.str(), StrEq(EXPECTED));
}

TEST(text_searializer_untyped_test, with_family) {
  constexpr auto EXPECTED = R"(# HELP total_count Total Request count.
# TYPE total_count untyped
total_count 1

)";

  metric_registry reg;

  auto tag_family = reg.make_family(metric_family::builder("total_count", "Total Request count."));
  auto &total_untyped_family = reg.make_untyped(tag_family);
  auto &total_counter = total_untyped_family.add();

  total_counter.inc();

  std::stringstream str;
  text_serializer::write(str, reg);

  ASSERT_THAT(str.str(), StrEq(EXPECTED));
}

