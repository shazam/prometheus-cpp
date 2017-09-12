#pragma once

#include <atomic>

namespace prometheus {

/**
 * busy wait for a "mutex" when it's expected that no one person will hold it for long
 */
class spin_lock {
  std::atomic_bool latch{false};

 public:
  void lock() {
    bool unlatched = false;
    while (!latch.compare_exchange_weak(unlatched, true, std::memory_order_acquire)) {
      unlatched = false;
    }
  }

  void unlock() {
    latch.store(false, std::memory_order_release);
  }
};

} // namespace prometheus