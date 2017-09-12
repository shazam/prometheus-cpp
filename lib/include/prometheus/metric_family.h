#pragma once

#include <prometheus/metric_labels.h>

namespace prometheus {

class metric_family {
 public:

  class builder {
   public:
    explicit builder(std::string name);

    builder &with_help(const std::string &help);

    builder &with_labels(const std::map<std::string, std::string> &labels);

    std::string name;
    std::string help;
    std::map<std::string, std::string> labels;
  };

  metric_family(const builder &builder);

  inline bool has_help() const {
    return !help.empty();
  }

  inline bool has_labels() const {
    return !labels.empty();
  }

  const std::string name;
  const std::string help;
  const metric_labels labels;
};

} // namespace prometheus