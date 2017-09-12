#pragma once

#include <cassert>

namespace prometheus {

// TODO: Should this throw?
#define PROMETHEUS_CHECK(x) assert(x)

}