#include <text_serializer.h>

namespace prometheus {

void metric_registry::write(std::ostream &os) const {
  std::lock_guard<std::mutex> guard(lock);
  text_serializer::write(os, *this);
}

} // namespace prometheus