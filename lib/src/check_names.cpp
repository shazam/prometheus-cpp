#include <check_names.h>

#include <regex>

namespace {

bool is_reserved(const std::string &name) {
  return name.compare(0, 2, "__") == 0;
}

} // namespace

namespace prometheus {

bool valid_metric_name(const std::string &name) {
  if (is_reserved(name)) {
    return false;
  }

  // see https://prometheus.io/docs/concepts/data_model/
  static const std::regex metric_name_regex("[a-zA-Z_:][a-zA-Z0-9_:]*");
  return std::regex_match(name, metric_name_regex);
}

bool valid_label_name(const std::string &name) {
  if (is_reserved(name)) {
    return false;
  }

  // see https://prometheus.io/docs/concepts/data_model/
  const std::regex label_name_regex("[a-zA-Z_][a-zA-Z0-9_]*");
  return std::regex_match(name, label_name_regex);
}

} // namespace prometheus
