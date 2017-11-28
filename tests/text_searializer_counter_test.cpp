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

TEST(text_searializer_counter_test, without_family) {
  constexpr auto EXPECTED = R"(# HELP total_count Total Request count.
# TYPE total_count counter
total_count 2.5

)";

  metric_registry reg;
  auto &total_counter_family = reg.make_counter(metric_family::builder("total_count", "Total Request count."));
  auto &total_counter = total_counter_family.add();
  total_counter.inc(2.5);

  std::stringstream str;
  text_serializer::write(str, reg);
  ASSERT_THAT(str.str(), StrEq(EXPECTED));
}

TEST(text_searializer_counter_test, with_family) {
  constexpr auto EXPECTED = R"(# HELP total_count Total Request count.
# TYPE total_count counter
total_count 1

)";

  metric_registry reg;

  auto &tag_family = reg.make_family(metric_family::builder("total_count", "Total Request count."));
  auto &total_counter_family = reg.make_counter(tag_family);
  auto &total_counter = total_counter_family.add();

  total_counter.inc();

  std::stringstream str;
  text_serializer::write(str, reg);

  ASSERT_THAT(str.str(), StrEq(EXPECTED));
}

TEST(text_searializer_counter_test, with_family_labels) {
  constexpr auto EXPECTED = R"(# HELP total_count Total Request count.
# TYPE total_count counter
total_count{label1="value1",label2="value2",} 1

)";

  metric_registry reg;

  auto &tag_family = reg.make_family(metric_family::builder("total_count", "total_count Total Request count.").with_labels(SOME_LABELS));

  auto &total_counter_family = reg.make_counter(tag_family);
  auto &total_counter = total_counter_family.add();

  total_counter.inc();

  std::stringstream str;
  text_serializer::write(str, reg);

  ASSERT_THAT(str.str(), StrEq(EXPECTED));
}

TEST(text_searializer_counter_test, with_timestamp) {
  constexpr auto EXPECTED = R"(# HELP total_count Total Request count.
# TYPE total_count counter
total_count{label1="value1",label2="value2",} 0 100000

)";

  metric_registry reg;

  auto &tag_family = reg.make_family(metric_family::builder("total_count", "total_count Total Request count.").with_labels(SOME_LABELS));

  auto &total_counter_family = reg.make_counter(tag_family);
  auto &total_counter = total_counter_family.add();

  total_counter.set_timestamp(std::chrono::milliseconds{100000});

  std::stringstream str;
  text_serializer::write(str, reg);

  ASSERT_THAT(str.str(), StrEq(EXPECTED));
}

TEST(text_searializer_counter_test, multi_family_metric) {
  constexpr auto EXPECTED = R"(# HELP total_count Total Request count.
# TYPE total_count counter
total_count{label1="value1",label2="value2",} 1
total_count{label1="value1",label2="value2",label3="value3",} 2

)";

  metric_registry reg;

  auto &tag_family = reg.make_family(metric_family::builder("total_count", "total_count Total Request count.").with_labels(SOME_LABELS));

  auto &total_counter_family = reg.make_counter(tag_family);
  auto &total_counter0 = total_counter_family.add();
  total_counter0.inc();

  auto &total_counter1 = total_counter_family.add({{"label3", "value3"}});
  total_counter1.inc(2);

  std::stringstream str;
  text_serializer::write(str, reg);

  ASSERT_THAT(str.str(), StrEq(EXPECTED));
}