// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "event_builder.h"
#include "ocudu/adt/spsc_queue.h"
#include <memory>
#include <vector>

namespace ocudu::schedtrace {

/// Pool of reusable flatbuffer event builders.
class event_builder_pool
{
public:
  /// Should be sized to hold the worst-case slot.
  static constexpr size_t default_buffer_size = 1024 * 32;

  explicit event_builder_pool(unsigned qsize, size_t initial_size = default_buffer_size) :
    max_buffer_size(initial_size), free_list(qsize + 2)
  {
    // Note: We add two more elements to the pool than the event queue capacity so that the producer never
    // fails to pop events from the free list.
    const unsigned cap = free_list.capacity();

    builders.reserve(cap);
    for (unsigned i = 0; i != cap; ++i) {
      builders.push_back(std::make_unique<event_builder>(initial_size));
      deallocate(*builders.back());
    }
  }
  event_builder_pool(const event_builder_pool&)            = delete;
  event_builder_pool(event_builder_pool&&)                 = delete;
  event_builder_pool& operator=(const event_builder_pool&) = delete;
  event_builder_pool& operator=(event_builder_pool&&)      = delete;

  /// \brief Called from the producer (RT) thread to get the next free builder. Builders in the free list are always
  /// clean, so this is a pure pop.
  event_builder* allocate()
  {
    event_builder* b;
    if (free_list.try_pop(b)) {
      return b;
    }
    return nullptr;
  }

  /// \brief Called from the consumer thread to return a builder to the pool.
  void deallocate(event_builder& b)
  {
    // A builder that grew kept the larger buffer, so the oversized event cost a single allocation on the producer
    // thread. Warn whenever the largest buffer size seen so far is exceeded, so the initial size can be revisited.
    if (b.buffer_size() > max_buffer_size) {
      fmt::print(stderr,
                 "Warning: schedtrace event outgrew its buffer ({} > {}) and the buffer was enlarged. Consider "
                 "increasing the initial size.\n",
                 b.buffer_size(),
                 max_buffer_size);
      max_buffer_size = b.buffer_size();
    }

    // Clear on the consumer thread so the producer (RT) thread finds ready-to-use builders in the free list.
    b.clear();
    [[maybe_unused]] const bool result = free_list.try_push(&b);
    ocudu_sanity_check(result, "Deallocation should never fail");
  }

  size_t capacity() const { return free_list.capacity(); }

  size_t size_approx() const { return free_list.size(); }

private:
  /// Largest builder buffer size seen so far, starting at the initial size. Growth beyond it triggers a warning.
  /// Accessed only by the consumer thread.
  size_t max_buffer_size;
  /// Pool of builders. Allocated once; addresses are stable for the lifetime of this object.
  std::vector<std::unique_ptr<event_builder>> builders;
  /// List of free builders that can be used by the producer.
  concurrent_queue<event_builder*, concurrent_queue_policy::lockfree_spsc> free_list;
};

} // namespace ocudu::schedtrace
