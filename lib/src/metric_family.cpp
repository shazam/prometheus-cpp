#include <prometheus/metric_family.h>
#include <prometheus/check.h>

#include <check_names.h>

namespace prometheus {

metric_family::builder::builder(std::string name) : name(std::move(name)) {
  PROMETHEUS_CHECK(valid_metric_name(this->name));
}

metric_family::builder &metric_family::builder::with_help(const std::string &help) {
  this->help = help;
  return *this;
}

metric_family::builder &metric_family::builder::with_labels(const std::map<std::string, std::string> &labels) {
  this->labels = labels;
  return *this;
}

metric_family::metric_family(const builder &builder) :
    name(builder.name),
    help(builder.help),
    labels(builder.labels) {
}

} // namespace prometheus