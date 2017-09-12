#pragma once

#include <string>

namespace prometheus {

bool valid_metric_name(const std::string &name);
bool valid_label_name(const std::string &name);

} // namespace prometheus